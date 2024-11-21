# FHE-HA
Accelerated blind evaluation of a binary decision tree from FHE, with a dockerfile to have the whole client-server-client workflow inside a unique docker

Here are the commands for running the code

*****************************************************************************************
FROM BDT (building and running a docker)

(docker build, then docker run with the option -it).

*****************************************************************************************
** FROM BDTClient/build (constructing a binary decision tree and input data, generating keys, encrypting, then sharing the keys and the data) **

./bdt_client_prefinal

cp ./demoData/cryptocontext.txt ../../BDTServer/build/demoData/cryptocontext.txt
cp ./demoData/key-public.txt ../../BDTServer/build/demoData/key-public.txt
cp ./demoData/key-eval-mult.txt ../../BDTServer/build/demoData/key-eval-mult.txt
cp ./demoData/e* ../../BDTServer/build/demoData/

cp ./demoData/cryptocontext.txt ../../BDTResult/build/demoData/cryptocontext.txt
cp ./demoData/key-private.txt ../../BDTResult/build/demoData/key-private.txt

******************************************************************************************
** FROM BDTServer/build (blind evaluating the binary decision tree with FHE, then sharing the encrypted result) **

./bdt-hom-eval 3

cp ./demoData/output_ciphertext.txt ../../BDTResult/build/demoData/output_ciphertext.txt

******************************************************************************************
** FROM BDTResult/build (decrypting the result) **

./bdt_result_prefinal

******************************************************************************************
