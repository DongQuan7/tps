#!/usr/bin/perl -w
##Write by jiandong.zhou@united-image.com

use strict;
use Cwd;
use File::Basename;

my $current_path     = getcwd();
my $protoGen_path    = "$current_path/protoGen.exe";
my $protoC_path      = "$current_path/protoc.exe";
my $cplusplus_output = "cplusplus";
my $csharp_output    = "csharp";

my $proto_head_file  = "$current_path/$cplusplus_output/rt_tps_proto_cmn.h";
my $proto_proj_name  = "RtTpsProtoLib";
my $proj_csharp_name = "UIH.RT.TPS.ProtoBuffer";

my $auto_copy_flag   = "0";  ##"1": auto copy, "0": not auto copy
my $h_file_cp_path   = "..\\include\\$proto_proj_name\\";
my $cc_file_cp_path  = "..\\$proto_proj_name\\";
my $cs_file_cp_path  = "..\\$proj_csharp_name\\";

my @protoGen_exec = (
    "$protoGen_path --include_imports --proto_path=./ -output_directory=$current_path/$csharp_output/",
    "$protoC_path --include_imports --cpp_out=$current_path/$cplusplus_output/",
);

my @ignore_proto_file = (
    #"rt_tms_msg_sample.proto",
);

sub run_cmds {
    my @cmds = @_;
    foreach (@cmds) {
        print "$_\n";
        system("$_");
    }
}


sub write_to_file {
    my($file_name, $content_ref) = @_;

    open(my $h, ">>", $file_name) || die "can not open $file_name : $!";
    foreach (@$content_ref) {
        print $h "$_\n";
    }

    close($h);
}

sub compile_file {
    my ($file) = @_;

    my $ignore_flag = 0;
    foreach my $ignore_file (@ignore_proto_file) {
        if ($file eq $ignore_file) {
            print "$file been ignore!\n";
            $ignore_flag = 1;
        }
    }

    if ($ignore_flag == 0) {
        print "compile: $file ...\n";
        foreach my $cmd (@protoGen_exec) {
            my $cmd_line = "$cmd $file";
            if (system($cmd_line) == 0) {
                print "run: $cmd_line success!\n";
            } else {
                print "run: $cmd_line failed!\n$!\n";
            }
        }
    }

    ##Generate common proto file tails
    my($filename, $directories, $suffix) = fileparse($file, ".proto");
    my @file_body = (
        "#include \"$proto_proj_name/$filename.pb.h\"",
    );

    write_to_file($proto_head_file, \@file_body);
}

sub pre_execute {
    my @pre_cmp_cmds = (
        "if not exist $cplusplus_output md $cplusplus_output",
        "if not exist $csharp_output md $csharp_output",
        "if exist $cplusplus_output\\*.h  del $cplusplus_output\\*.h",
        "if exist $cplusplus_output\\*.cc del $cplusplus_output\\*.cc",
        "if exist $csharp_output\\*.cs del $csharp_output\\*.cs",
    );

    run_cmds(@pre_cmp_cmds);

    ##Generate common proto file
    my @file_head = (
        "//This file is auto gen by script, Don`t edit it",
        "#ifndef RT_TPS_PROTO_CMN_H",
        "#define RT_TPS_PROTO_CMN_H",
        "",
        "#if defined (_MSC_VER)",
        "#pragma warning(push)",
        "#pragma warning(disable:4127 4100 4512 4244 4267 4125)",
        "#endif",
        "",
    );

    write_to_file($proto_head_file, \@file_head);
}

sub post_execute {
    ##Generate common proto file tails
    my @file_tail = (
        "",
        "#endif  //RT_TPS_PROTO_CMN_H",
    );

    write_to_file($proto_head_file, \@file_tail);

    if ($auto_copy_flag eq "1") {
        ##Post operate
        my @post_cmp_cmds = (
            "if exist $h_file_cp_path  copy /Y $cplusplus_output\\*.h $h_file_cp_path",
            "if exist $cc_file_cp_path copy /Y $cplusplus_output\\*.cc $cc_file_cp_path",
            "if exist $cs_file_cp_path copy /Y $csharp_output\\*.cs $cs_file_cp_path",
        );

        run_cmds(@post_cmp_cmds);
    }
}

sub execute {
    if ((-e $protoGen_path) and (-e $protoC_path)) { 
        opendir(my $dh, $current_path) || die "can't opendir $current_path: $!";
        my @files = readdir($dh);

        foreach my $file (@files) {
            if($file =~ m/\.proto$/) {  
                compile_file($file);
            }
        }
    } else {
        print "does not exist $protoGen_path or $protoC_path\n";
    }    
}

sub main {
    print "hello, tps\n";
    
    pre_execute();
    execute();
    post_execute();

    print "press any key for exit!\n";
    <>;
}

main();

