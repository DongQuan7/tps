%~d0
cd %~p0

.\protoGen.exe  --proto_path=.\ rt_tps_proto.proto
.\protoGen.exe  --proto_path=.\ rt_tps_layoutinfos.proto

.\protoGen.exe  --include_imports --proto_path=.\ rt_tps_course.proto
.\protoGen.exe  --include_imports --proto_path=.\ rt_tps_try.proto
.\protoGen.exe  --include_imports --proto_path=.\ rt_tps_plan.proto
.\protoGen.exe  --include_imports --proto_path=.\ rt_tps_beam.proto
.\protoGen.exe  --include_imports --proto_path=.\ rt_tps_mprpageturn.proto
.\protoGen.exe  --include_imports --proto_path=.\ rt_tps_beam_outline.proto
.\protoGen.exe  --include_imports --proto_path=.\ rt_tps_manual_fusion.proto
.\protoGen.exe  --include_imports --proto_path=.\ rt_tps_pseudo_color.proto
.\protoGen.exe  --include_imports --proto_path=.\ rt_tps_voi_advanced.proto
