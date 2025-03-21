english_dev:
    directory for grammar development
    It can be tested using the shell script
    bin/devvikefxip <xml_file>


english: directory for the encrypted version of the grammar (to be installed on the external server)
    It can be tested using the shell script
    bin/vikefxip <xml_file>


Creation of the encrypted version from the dev version:

    - cd to data/
    - run:    ./install
    - then english contains the encrypted version to be installed (along with NTMDATA) on the external server
    
    
