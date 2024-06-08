cmd_/home/zw/work/osmd/Module.symvers := sed 's/\.ko$$/\.o/' /home/zw/work/osmd/modules.order | scripts/mod/modpost     -o /home/zw/work/osmd/Module.symvers -e -i Module.symvers   -T -
