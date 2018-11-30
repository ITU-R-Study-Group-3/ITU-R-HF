Applications packaged in the 'snap' format are self-contained and designed to work across a wide range of Linux distributions.

## Limitations
Snaps restrict access to the network and files with plugs, defining access to the host system.  The iturhfprop snap
restricts access to the snap itself and the users home directory (except for hidden directories (beginning with a '.').  This
prevents a snap from accessing ssh keys etc.

## Data files
The snap contains the data files required to run ITURHFProp.  The DataFilePath parameters should be set as follows;

    DataFilePath "/snap/iturhfprop/current/usr/share/iturhfprop/data/"

## Building the Snap
 Create an empty working directory.
 Create a sub-directory called 'snap'.
 Copy the file snapcraft.yaml to the snap directory.

     my-working-directory
         snap
             snapcraft.yaml

Edit the snapcraft.yaml file to set the parts:iturhfprop:source: parameter to the
correct value. Alternatively, this can be set to the git repo with the following
entry;

    parts:
      iturhfprop:
          source: https://github.com/onetablespoon/ITU-R-HF.git

Install the snap with the following command (the --dangerous argument is required
as the package is unsigned);

    sudo snap install --dangerous iturhfprop_0.1_amd64.snap
