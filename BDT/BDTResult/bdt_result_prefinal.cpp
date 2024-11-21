//HOMOMORPHIC EVALUATION OF BINARY DECISION TREE FROM OPENFHE : RESULT DECRYPTION

#include "openfhe.h"

// header files needed for serialization
#include "ciphertext-ser.h"
#include "cryptocontext-ser.h"
#include "key/key-ser.h"
#include "scheme/bgvrns/bgvrns-ser.h"

using namespace lbcrypto;

const std::string DATAFOLDER = "demoData";

int main()
{
	//getting the crypto-context
	CryptoContext<DCRTPoly> cc;
    if (!Serial::DeserializeFromFile(DATAFOLDER + "/cryptocontext.txt", cc, SerType::BINARY)) {
        std::cerr << "I cannot read serialization from " << DATAFOLDER + "/cryptocontext.txt" << std::endl;
        return 1;
    }
    std::cout << "The cryptocontext has been deserialized." << std::endl;
    
    //getting the secret key
    PrivateKey<DCRTPoly> sk;
    if (Serial::DeserializeFromFile(DATAFOLDER + "/key-private.txt", sk, SerType::BINARY) == false) {
        std::cerr << "Could not read secret key" << std::endl;
        return 1;
    }
    std::cout << "The secret key has been deserialized." << std::endl;
    
    //getting the encrypted result
	Ciphertext<DCRTPoly> output_ciphertext;
    if (Serial::DeserializeFromFile(DATAFOLDER + "/output_ciphertext.txt", output_ciphertext, SerType::BINARY) == false) {
        std::cerr << "Could not read the ciphertext" << std::endl;
        return 1;
    }
    std::cout << "The encrypted result of the homomorphic evaluation has been deserialized." << std::endl;
    
    //decrypting the result
	Plaintext final_output;
	cc->Decrypt(sk, output_ciphertext, &final_output);
	std::cout << "OUTPUT VALUE : " << final_output << std::endl;
	
	//main return value
	return 0;
}
