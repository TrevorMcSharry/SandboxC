# SandboxC
Sandbox is a program that creates a terminal like environment where the user can run commands and programs with resource limits. The user can also redirect input and output to files. Programs can be run in the background using & and tracked using 'jobs'.


`tmcsharr@sandbox: ~/cs360/Sandbox> ls
args  args.c  exec  exec.c  input.txt  output.txt  sandbox  sandbox.c  sleepy  valgrind-out.txt
tmcsharr@sandbox: ~/cs360/Sandbox> ls -la
total 136
drwx------.  2 tmcsharr tmcsharr   163 Dec  5 21:14 .
drwxr-xr-x. 13 tmcsharr tmcsharr   178 Dec 13 15:16 ..
-rwxrwxrwx.  1 tmcsharr tmcsharr 18096 Nov 22 15:20 args
-rw-------.  1 tmcsharr tmcsharr   148 Nov 27 10:38 args.c
-rwx------.  1 tmcsharr tmcsharr 18200 Nov 22 15:28 exec
-rw-------.  1 tmcsharr tmcsharr   251 Nov 22 15:29 exec.c
-rw-------.  1 tmcsharr tmcsharr     5 Nov 30 14:02 input.txt
-rw-------.  1 tmcsharr tmcsharr    87 Nov 30 15:03 output.txt
-rwx------.  1 tmcsharr tmcsharr 28976 Dec  5 21:13 sandbox
-rw-------.  1 tmcsharr tmcsharr  9894 Dec 21 16:10 sandbox.c
-rwx------.  1 tmcsharr tmcsharr 18320 Nov 25 18:12 sleepy
-rw-------.  1 tmcsharr tmcsharr 14421 Dec  5 21:15 valgrind-out.txt
tmcsharr@sandbox: ~/cs360/Sandbox> ./sleepy 10
tmcsharr@sandbox: ~/cs360/Sandbox> ./sleepy 10 &
tmcsharr@sandbox: ~/cs360/Sandbox> jobs
1 job
   1732143  - jobs
tmcsharr@sandbox: ~/cs360/Sandbox> whoami
tmcsharr
tmcsharr@sandbox: ~/cs360/Sandbox> echo $USER
tmcsharr
tmcsharr@sandbox: ~/cs360/Sandbox> 
`
