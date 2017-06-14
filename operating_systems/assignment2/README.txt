To run the code in Linux:
- Make the present working directory to the unzipped folder(use "pwd" to check)
- Once in the right directory type as "make" which creates kernel object and test file
- Then as root user type "(sudo)insmod test.ko" which inserts the device to kernel
- "ls/dev" lists all the devices loaded to Kernel (check whether chardev is loaded)
- tail /var/log/syslog tells the steps that is
	--> Initiating the Character device
	--> Device loaded successfully
	--> Class registered successfully
	--> Device class created
- Then type "sudo ./utest" (runs the test file)
- Now if you type tail /var/log/syslog 
	--> Device has been opened
	--> Reads the test file (read operation)
	--> Device unloaded successfully (read operation is over)
- To view the full log type "dmesg" where the pid ppid cpu and state are seperated by comma..Example 5847,1180,0,1
- Now type "sudo rmmod test" to remove the device
- Now type tail /var/log/syslog
	--> Exit from the module

