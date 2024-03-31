
Modules prefixes:

  - 000-010 deployer
  - 020-099 nodeinit
  - 100-110 ci-build modules
  - 120-199 cluster modules
  - 200-... reserver

  Modules with the same prefix are mutually exclusive.
  This means that only one of these modules can be used at a time,
  even though they will both be added to the deployer image.

OS support:

  - ci-build: Ubuntu 22.04+
  - cluster nodes: Ubuntu 22.04+

To get packages listed in packages.txt run:

  ./get-repo.sh
  ./get-pkg.sh
