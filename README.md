# SandboxC
Sandbox is a program that creates a terminal like environment where the user can run commands and programs with resource limits. The user can also redirect input and output to files. Programs can be run in the background using & and tracked using 'jobs'.


`tmcsharr@sandbox: ~/cs360/Sandbox> ls
args  args.c  exec  exec.c  input.txt  output.txt  sandbox  sandbox.c  sleepy  valgrind-out.txt
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
