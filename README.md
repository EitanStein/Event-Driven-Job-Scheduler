# Event-Driven-Job-Scheduler

### About the project
this is a learning project to play around with linux fork/exec and socket communcation 

Clients can send commands to the server (manager) to run them (WIP to allow for CLI input commands)
Manager (server) receives said commands and manages which jobs are handled first based on priority and resource reqs (memory and cpu - input params from the client)
Manager then forks a child process to run the job via execvp

there is no plan to add encryption at this point

the job scheduling is handled as follows:
find the highest priority that currently has pending jobs
run the first job you find from said priority with enough resources for it
for highest priority - only run the first job in the queue to avoid heavy job starvation

WIP adding time stamps to jobs and use it to increase priority of older jobs
I have yet to decide if there will be a dedicated urgent priority that can only be from client input that must be run first