//HOMOMORPHIC EVALUATION OF BINARY DECISION TREE FROM OPENFHE : CLIENT SIDE

#include "openfhe.h"

// header files needed for serialization
#include "ciphertext-ser.h"
#include "cryptocontext-ser.h"
#include "key/key-ser.h"
#include "scheme/bgvrns/bgvrns-ser.h"

using namespace lbcrypto;

const std::string DATAFOLDER = "demoData";

//binary decision trees
typedef struct bdt
{
	std::vector<int64_t> root;
	bdt* left;
	bdt* right;
} bdt;

//splitting a vector in two (in order to build a tree)
std::vector<std::vector<std::vector<int64_t>>> split(std::vector<std::vector<int64_t>> tags, int depth)
{
	std::vector<std::vector<std::vector<int64_t>>> result;
	result.push_back({});
	result.push_back({});

	int cpt = 1;

	
	for(int i=1; i<depth; i++)
	{
		for(int j=0; j<pow(2, i-1); j++)
		{
			(result[0]).push_back(tags[cpt]);
			cpt++;
		}
		for(int j=0; j<pow(2, i-1); j++)
		{
			(result[1]).push_back(tags[cpt]);
			cpt++;
		}
	}
	
	return result;
}

//building a tree from a vector
bdt build_tree(std::vector<std::vector<int64_t>> tags, int depth)
{
	bdt tree;
	tree.root = tags[0];
	std::cout << tree.root << std::endl;
	tree.left = new bdt();
	tree.right = new bdt();
	if(depth>1)
	{
	   *(tree.left) = build_tree(split(tags, depth)[0], depth-1);
	   *(tree.right) = build_tree(split(tags, depth)[1], depth-1);
	}
	else
	{
		tree.left = NULL;
		tree.right = NULL;
	}
	return tree;
}

//binary decision trees encoded as plaintexts
typedef struct bdt_pt
{
	Plaintext root;
	bdt_pt* left;
	bdt_pt* right;
} bdt_pt;

//encrypted binary decision trees
typedef struct bdt_ct
{
	Ciphertext<DCRTPoly> root;
	bdt_ct* left;
	bdt_ct* right;
} bdt_ct;

//encoding a binary decision tree
bdt_pt bdt_encode(CryptoContext<DCRTPoly> cc, bdt tree)
{
	bdt_pt result;
	result.root = cc->MakePackedPlaintext(tree.root);
	result.left = new bdt_pt();
	result.right = new bdt_pt();
	if(tree.left!=NULL)
	{
	  *(result.left) = bdt_encode(cc, *(tree.left));
	}
	else
	{
	  result.left = NULL;
	}
	if(tree.right!=NULL)
	{
	  *(result.right) = bdt_encode(cc, *(tree.right));
	}
	else
	{
	  result.right=NULL ;
	}
	return result;
}

//encryption of a binary decision tree
bdt_ct bdt_encrypt(CryptoContext<DCRTPoly> cc, bdt_pt tree, const PublicKey<DCRTPoly> pk)
{
	bdt_ct result;
	result.root = cc->Encrypt(pk, tree.root);
	result.left = new bdt_ct();
	result.right = new bdt_ct();
	if(tree.left!=NULL)
	{
	  *(result.left) = bdt_encrypt(cc, *(tree.left), pk);
	}
	else
	{
	  result.left = NULL;
	}
	if(tree.right!=NULL)
	{
	  *(result.right) = bdt_encrypt(cc, *(tree.right), pk);
	}
	else
	{
	  result.right = NULL;
	}
	return result;
}

// subfunction for recursive serialization of an encrypted bdt (from depth-first search)
int ebdt_serialize_switched(bdt_ct tree, std::string name, int i)
{
	if (!Serial::SerializeToFile(DATAFOLDER + "/" + name + std::to_string(i) + ".txt", tree.root, SerType::BINARY)) {
        std::cerr << "Error writing serialization of node " << i << "to ciphertext" << i << ".txt" << std::endl;
    }
    std::cout << "serialized ciphertext " << i << std::endl;
    i++;
    
    if(tree.left !=NULL)
    {
	   i = ebdt_serialize_switched(*(tree.left), name, i);	
    }
    //i++; (?)
    
    if(tree.right !=NULL)
    {
		i = ebdt_serialize_switched(*(tree.right), name, i);	
    }
    //i++; (?)
    
    return i;
}

// serialization of an encrypted bdt from depth-first search (ebdt_serialize_switched with i=0 and no output value)
void ebdt_serialize(bdt_ct tree, std::string name)
{
	ebdt_serialize_switched(tree, name, 0);
}

// sub-function for recursive de-serialization of an encrypted bdt
void ebdt_deserialize_switched(bdt_ct *tree, std::string name, int depth, int *tag)
{
	if (Serial::DeserializeFromFile(DATAFOLDER + "/" + name + std::to_string(*tag) + ".txt", tree->root, SerType::BINARY) == false) {
        std::cerr << "Could not read the ciphertext" << std::endl;
    }
    std::cout << "a ciphertext has been deserialized." << std::endl;
    
    (*tag)++;
    
    tree->left = new bdt_ct();
	tree->right = new bdt_ct();
    
    if(depth !=1)
    {
		ebdt_deserialize_switched(tree->left, name, depth-1, tag);
		ebdt_deserialize_switched(tree->right, name, depth-1, tag);
	}
	else
	{
		tree->left = NULL;
		tree->right = NULL;
	}
}    

/////////////////////////////////////////////
//                                         //
//               |MAIN|                    //
//                                         //
/////////////////////////////////////////////

int main()
{
      //cryptocontext setting
      CCParams<CryptoContextBGVRNS> parameters;
      parameters.SetMultiplicativeDepth(8);
      parameters.SetPlaintextModulus(65537);
      
      CryptoContext<DCRTPoly> cc = GenCryptoContext(parameters);
      cc->Enable(PKE);
      cc->Enable(KEYSWITCH);
      cc->Enable(LEVELEDSHE);
      
      //key generation
      KeyPair<DCRTPoly> keyPair;
      keyPair = cc->KeyGen();
      const PublicKey<DCRTPoly> pk = keyPair.publicKey;
      const PrivateKey<DCRTPoly> sk = keyPair.secretKey;
      
      cc->EvalMultKeyGen(sk);
      
      // Serialize cryptocontext
      if (!Serial::SerializeToFile(DATAFOLDER + "/cryptocontext.txt", cc, SerType::BINARY)) {
          std::cerr << "Error writing serialization of the crypto context to "
                       "cryptocontext.txt"
                    << std::endl;
          return 1;
      }
      std::cout << "The cryptocontext has been serialized." << std::endl;
      
      // Serialize the public key
      if (!Serial::SerializeToFile(DATAFOLDER + "/key-public.txt", keyPair.publicKey, SerType::BINARY)) {
          std::cerr << "Error writing serialization of private key to key-public.txt" << std::endl;
          return 1;
      }
      std::cout << "The public key has been serialized." << std::endl;
      
      // Serialize the secret key
      if (!Serial::SerializeToFile(DATAFOLDER + "/key-private.txt", keyPair.secretKey, SerType::BINARY)) {
          std::cerr << "Error writing serialization of private key to key-private.txt" << std::endl;
          return 1;
      }
      std::cout << "The secret key has been serialized." << std::endl;
      
      // Serialize the relinearization (evaluation) key for homomorphic
      // multiplication
      std::ofstream emkeyfile(DATAFOLDER + "/" + "key-eval-mult.txt", std::ios::out | std::ios::binary);
      if (emkeyfile.is_open()) {
          if (cc->SerializeEvalMultKey(emkeyfile, SerType::BINARY) == false) {
              std::cerr << "Error writing serialization of the eval mult keys to "
                           "key-eval-mult.txt"
                        << std::endl;
              return 1;
          }
          std::cout << "The eval mult keys have been serialized." << std::endl;

          emkeyfile.close();
      }
      else {
          std::cerr << "Error serializing eval mult keys" << std::endl;
          return 1;
      }
      
      //////////////////////////////////////////////////////
	  // ENCRYPTING A BINARY DECISION TREE //
	  //////////////////////////////////////////////////////
	  
	  //constructing a tree
      bdt tree;
      
      //(nodes)
      tree.root = {1};
      
      tree.left = new bdt();
      tree.right = new bdt();
      (tree.left)->root = {1};
      (tree.right)->root = {0};
      
      
      (tree.left)->left = new bdt();
      (tree.left)->right = new bdt();
      (tree.right)->left = new bdt();
      (tree.right)->right = new bdt();
      (tree.left)->left->root = {0};
      (tree.left)->right->root = {1};
      (tree.right)->left->root = {0};
      (tree.right)->right->root = {1};
       
      //(leaves)
      (tree.left)->left->left = NULL;
      (tree.left)->left->right = NULL;
      (tree.left)->right->left = NULL;
      (tree.left)->right->right = NULL;
      (tree.right)->left->left = NULL;
      (tree.right)->left->right = NULL;
      (tree.right)->right->left = NULL;
      (tree.right)->right->right = NULL;
      
      std::cout << "nodes of the binary decision tree (step by step and from left to right)" << std::endl;
      
      // printing the nodes
      std::cout << tree.root << (tree.left)->root << (tree.right)->root << (tree.left)->left->root << (tree.left)->right->root << (tree.right)->left->root << (tree.right)->right->root << std::endl;
      
      //encoding
      bdt_pt encoded_tree = bdt_encode(cc, tree);
      
      //encrypting
      bdt_ct encrypted_tree = bdt_encrypt(cc, encoded_tree, pk);
      
      //serialization
      ebdt_serialize(encrypted_tree, "encrypted_tree");
      
      //////////////////////////////////////////////////////
	  // ENCRYPTING INPUT DATA AS ANOTHER TREE //
	  //////////////////////////////////////////////////////
	  
      // constructing input data as another tree
	  bdt data;
      
      //nodes
      data.root = {0};
      
      data.left = new bdt();
      data.right = new bdt();
      (data.left)->root = {1};
      (data.right)->root = {1};
      
      
      (data.left)->left = new bdt();
      (data.left)->right = new bdt();
      (data.right)->left = new bdt();
      (data.right)->right = new bdt();
      (data.left)->left->root = {0};
      (data.left)->right->root = {1};
      (data.right)->left->root = {1};
      (data.right)->right->root = {0};
       
      //(leaves)
      (data.left)->left->left = NULL;
      (data.left)->left->right = NULL;
      (data.left)->right->left = NULL;
      (data.left)->right->right = NULL;
      (data.right)->left->left = NULL;
      (data.right)->left->right = NULL;
      (data.right)->right->left = NULL;
      (data.right)->right->right = NULL;
	
      std::cout << "nodes of the binary data tree (step by step and from left to right)" << std::endl;
      
      // printing the nodes
      std::cout << data.root << (data.left)->root << (data.right)->root << (data.left)->left->root << (data.left)->right->root << (data.right)->left->root << (data.right)->right->root << std::endl;
	  
	  //encoding
	  bdt_pt encoded_data = bdt_encode(cc, data);
	  
	  //encrypting
	  bdt_ct encrypted_data = bdt_encrypt(cc, encoded_data, pk);
	  
	  //serialization
      ebdt_serialize(encrypted_data, "encrypted_data");
      
      //////////////////////////////
      //////////////////////////////
      
      //main return value
      return 0;
}
