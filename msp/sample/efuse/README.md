1. This test is used to verify the read and write functions of the efuse module. Since this case will write to the OTP unit inside the chip, it is recommended to test carefully (you can only use one machine to test, do not test every machine);
The efuse inside the AX650 has 128 blocks, each block is 32 bits. This test program tests the read and write operations of block30, block66-76, and block78-94 that can be used by customers;

2. Operation method:
#sample_efuse

3. The normal operation log output results are as follows:
This will write the Efuse, please confirm you want Do it!!!
Efuse test PASS!

4. If the efuse of the chip has already performed this test, and the test is performed again, the following error will be reported. This is a normal phenomenon. The OTP of each chip can only be written once;
This will write the Efuse, please confirm you want Do it!!!
Efuse test write error 1e, 80050081
