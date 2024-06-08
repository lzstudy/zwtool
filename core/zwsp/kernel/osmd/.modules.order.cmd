cmd_/home/zw/work/osmd/modules.order := {   echo /home/zw/work/osmd/osmd.ko; :; } | awk '!x[$$0]++' - > /home/zw/work/osmd/modules.order
