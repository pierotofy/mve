find /data/drone/brighton2/mve -name *L5* -delete
OMP_NUM_THREADS=20 ~/Documents/mve/apps/dmrecon/dmrecon -s5 --progress=silent --local-neighbors=2 /data/drone/brighton2/mve
while [ $? -ne 1 ]; do
    find /data/drone/brighton2/mve -name *L5* -delete
    OMP_NUM_THREADS=20  ~/Documents/mve/apps/dmrecon/dmrecon -s5 --progress=silent --local-neighbors=2 /data/drone/brighton2/mve
done
