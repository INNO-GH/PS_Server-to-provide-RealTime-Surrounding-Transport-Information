# Server to provide RSTI



## Add and Update

Add
```
git remote add origin https://github.com/INNO-GH/PS_Server-to-provide-RealTime-Surrounding-Transport-Information.git
git clone -b master https://github.com/INNO-GH/PS_Server-to-provide-RealTime-Surrounding-Transport-Information.git
```

Update
```
cd PS_Server-to-provide-RealTime-Surrounding-Transport-Information
git pull origin master
```

## Run Server and Client

Server
- [ ] change port number in Communicator.cpp code according to the environment.
```
cd Server
mkdir build
cd build
cmake ..
make
./Server
```

Client
- [ ] change url in Client.html code according to the environment.
- [ ] open Client.html file.
- [ ] click two point on the map. (first click is location, and second click makes angle with first)
- [ ] check the result. (there could be long delay according to the status of OpenAPI.)
