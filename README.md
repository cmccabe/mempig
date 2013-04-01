Mempig
======================
Mempig is a program which consumes memory.  It mlocks this memory, making it
unusable for other processes on the system.

This is helpful if you want to benchmark performance with different amounts of
system memory, but don't want to get out the screwdriver.

How do I build the source code?
----------------------------------
    ./configure
    make
    sudo make install

What if I get "permission denied"?
----------------------------------
You can either run as root, or increase the amount of memory your user can
mlock with ulimit.

What do the options do? 
----------------------------------
* \-a sets the amount of memory to consume in bytes.

* \-d tells mempig to daemonize.  When mempig is daemonized, it will be
  detached from the session that ran it.  This is useful if, for example, you
  are running mempig on many cluster nodes, and don't want to keep an ssh
  session open to each one.

  Once mempig is running as a daemon, you will have to use kill or killall to
  kill it, because it is detached from the foreground session.

* \-h displays the help message 

* \-n skips the populate stage, letting the locked memory stay initialized 
   to 0.  This is not recommended.

How do I know mempig is working?
----------------------------------
On Linux, at least, you can find out how much memory is mlocked using:
    grep Mlock /proc/meminfo

You should see this amount increase when mempig is running.
