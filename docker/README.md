# mainCraft Docker

This is a tiny documentation about the mainCraft Docker and related things.

## Dockerfile

This Dockerfile contains the minimun necessary to compile and run the mainCraft.
To generate a container image use this command:

``` console
sudo docker build --tag maincraft:1.0 .
```

### Image leftover

After this you may want to delete the leftover image that this Dockerfile
generates. Use the following command to get rid of it:

``` console
sudo docker image prune
```

### Docker Fail to generate

Sometimes the curl command fails and the docker gets stucked. To solve this issue
you need to use `--no-cache` flag.

## Dock.sh

This is script aims to help users and developers to compile and run the mainCraft
insider a docker container.
To use it you need pass the name of container image and the command that you want
execute inside the container, as demonsted in this example:

``` console
./dock.sh -c mainCraft:1.0 -i 'make all'
```

