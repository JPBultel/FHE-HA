# FHE-HA Integrated Workflow
Blind evaluation of a binary decision tree from FHE with GPU acceleration, in a dockerfile to have the whole client-server-client workflow inside a unique docker.

### 1. Environment preparation:
Eventhough the application is dockerized, it relies on real hardware (NVIDIA GPU), hence the host environment should contain an NVIDIA GPU, the compatible NVIDIA GPU Driver, the NVIDIA Container Toolkit as well as the docker engine properly configured.

##### 1.1 NVIDA driver:
`TODO`
##### 1.2 NVIDIA Container Toolkit:
Installation:
```
sudo apt-get update
sudo apt-get install -y nvidia-container-toolkit
```
##### 1.3 Docker Configuration:
Modify `/etc/docker/daemon.json` file on the host so that Docker can use the NVIDIA Container Runtime:
```
sudo nvidia-ctk runtime configure --runtime=docker
```
Restart the Docker daemon:
```
sudo systemctl restart docker
```
### 2. Clone & Build the Workflow:
```
git clone git@github.com:JPBultel/FHE-HA.git
cd FHE-HA/BDT/
docker build -t heha .
```

### 3. Run
3.1 Run the container (will result in the container terminal):
```
sudo docker run --gpus all -it --name heha heha
```
3.2 Construct a binary decision tree and input data, generate keys, encrypt:
```
cd BDTClient/build
./bdt_client_prefinal
```
3.3 Share the keys and the data:
```
cp ./demoData/cryptocontext.txt ../../BDTServer/build/demoData/cryptocontext.txt &&\
cp ./demoData/key-public.txt ../../BDTServer/build/demoData/key-public.txt &&\
cp ./demoData/key-eval-mult.txt ../../BDTServer/build/demoData/key-eval-mult.txt &&\
cp ./demoData/e* ../../BDTServer/build/demoData/ &&\
cp ./demoData/cryptocontext.txt ../../BDTResult/build/demoData/cryptocontext.txt &&\
cp ./demoData/key-private.txt ../../BDTResult/build/demoData/key-private.txt
```

3.4 Run the application (blind evaluation of the binary decision tree with FHE):
```
cd ../../BDTServer/build/
./bdt-hom-eval 3
```
3.5 Share the encrypted result:
```
cp ./demoData/output_ciphertext.txt ../../BDTResult/build/demoData/output_ciphertext.txt
```
3.6 Decrypt the result:
```
cd ../../BDTResult/build/
./bdt_result_prefinal
```

****************************************************************************************
****************************************************************************************
(FORMER README INSTRUCTIONS) ***********************************************************
****************************************************************************************
# FHE-HA
Accelerated blind evaluation of a binary decision tree from FHE, with a dockerfile to have the whole client-server-client workflow inside a unique docker

Here are the commands for running the code

*****************************************************************************************
FROM BDT (building and running a docker)

(docker build, then docker run with the option -it).

*****************************************************************************************
** FROM BDTClient/build (constructing a binary decision tree and input data, generating keys, encrypting, then sharing the keys and the data) **

./bdt_client_prefinal

*********

cp ./demoData/cryptocontext.txt ../../BDTServer/build/demoData/cryptocontext.txt

cp ./demoData/key-public.txt ../../BDTServer/build/demoData/key-public.txt

cp ./demoData/key-eval-mult.txt ../../BDTServer/build/demoData/key-eval-mult.txt

cp ./demoData/e* ../../BDTServer/build/demoData/

********

cp ./demoData/cryptocontext.txt ../../BDTResult/build/demoData/cryptocontext.txt

cp ./demoData/key-private.txt ../../BDTResult/build/demoData/key-private.txt


******************************************************************************************
** FROM BDTServer/build (blind evaluating the binary decision tree with FHE, then sharing the encrypted result) **

./bdt-hom-eval 3

******

cp ./demoData/output_ciphertext.txt ../../BDTResult/build/demoData/output_ciphertext.txt

******************************************************************************************
** FROM BDTResult/build (decrypting the result) **

./bdt_result_prefinal

******************************************************************************************