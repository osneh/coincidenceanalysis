# coincidenceAnalysis



## Getting started

```
git clone https://gitlab.in2p3.fr/h.abreu/coincidenceanalysis.git
cd coincidenceanalysis
cd clustering
source kcompile.sh
```



## To run
```
cd ../
```
provide your picmic Input-file in ascii format and your Output directory 
source run.bat inFilePicmic OutDir

for instance,
``` 
source run.bat data/sampic_run27/picmic_data/picmic_data_20240721_23_44_19.txt data/sampic_run27/sampic_run27.dat /home/habreu/WORK/coincidenceanalysis/data/
```
it will create, the plots for the picmic crossing strips and an outputfile with the results (time coincidence with the Sampic channels + picmic centroid coordinates)

## Integrate with your tools

- [ ] [Set up project integrations](https://gitlab.in2p3.fr/h.abreu/coincidenceanalysis/-/settings/integrations)
