%~d0
cd %~p0

.\protoc.exe --cpp_out=.\ %1
.\protoGen.exe --proto_path=.\ %1



