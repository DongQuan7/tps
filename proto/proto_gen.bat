%~d0
cd %~p0

.\protoc.exe --cpp_out=.\ rt_tps_layoutinfos.proto
.\protoc.exe --cpp_out=.\ rt_tps_mprpageturn.proto
.\protoc.exe --cpp_out=.\ rt_tps_beam_outline.proto
.\protoc.exe --cpp_out=.\ rt_tps_beam.proto
.\protoc.exe --cpp_out=.\ rt_tps_manual_fusion.proto
.\protoc.exe --cpp_out=.\ rt_tps_pseudo_color.proto
.\protoc.exe --cpp_out=.\ rt_tps_voi_advanced.proto
