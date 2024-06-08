cmd_/home/zw/work/osmd/03_module/modules.order := {   echo /home/zw/work/osmd/03_module/osmd.ko; :; } | awk '!x[$$0]++' - > /home/zw/work/osmd/03_module/modules.order
