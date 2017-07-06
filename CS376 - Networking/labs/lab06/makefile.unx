#
# Standard Makefile for Ultrix 4.x VAX
#                       AlphaStation DECUnix 4.x:
#			RedHat Linux 5.x/6.x on PC or on AlphaStation: 
#				(other Linuxes may require some 
#				minor adjustments)
#			Sun Solaris 7/8 with gcc: (you MUST first put 
#				a script /usr/local/bin/machine
#				which, when run, simply echos "sun")
#			PowerbookG4 OSX/Darwin(bsd) with 
#				gcc: (machine returns "ppc7450")
#
# for programs which
#
#		1. possibly access tcp/ip sockets 
#		2. possibly require POSIX threads.
#
# This Makefile assumes each platform has "machine" and "uname"
# commands which return values as in the table below (note: on a 
# Sun machine you may have to add a script /usr/local/bin/machine
# which simply echoe "sun")
#
# platform	oper_sys	`machine`	`uname`
# ------------------------------------------------------------------
# Vax		Ultrix 4.x	 vax		ULTRIX
# AlphaStation	Tru64Unix	 alpha		OSF1
# Sun		Solaris 7/8	 sun		SunOS
# PowerbookG4	OSX/Darwin(bsd)	 ppc740		Darwin
# IntelPC	Linux		<nothing>	Linux
# AlphaStation	Linux		<nothing>	Linux
# ------------------------------------------------------------------
#
# It stores information for each machine architecture in tmpfiles 
# (of the form "TMP.._$(MACHINE)") then using the "machine" command 
# (via MACHINE=`machine`) it copies the correct tmpfile for the 
# platform and defines the three macros:
#
# 	CC=`cat TMPCC`			compiler invocation
# 	CFLAGS=`cat TMPFLAGS`		compiler (and debug) flags
# 	LFLAGS=`cat TMPLD`		linker flags
#
# ------------------------------------------------------------------
#
# for the following tcp/ip utility programs:
#
#		process.c
#		gethost.c
#		pipesend.c
#		vcsend.c and vcrec.c
#		dgrmsend.c and dgrmrec.c
#		daemon.c
#		s_tlnt.c s_daemon.c and s_sh.c
#		ngraph.c (datafile: ngraph.dat)
#
# (note that the following code is platform-specific: s_getty.c
#	s_tndesq.c prc.c and probably OBSOLETE)
#
# ------------------------------------------------------------------
# general defines for debugging
# ------------------------------------------------------------------
#
MACHINE=`machine`
OPSYS=`uname`
#
CDEBUG= -DDEBUG -g
#
# set higher level of debug (useful for dl_utils.c)
# CDEBUG= -DDEBUG -g -DPF_DEBUG
#
# set higher level of debug (useful for digdnsq.c)
# CDEBUG= -DDEBUG -DDEBUG2 -g
#
# ------------------------------------------------------------------
# architecture-specfic defines: vax alpha sun ppc740 linux
# ------------------------------------------------------------------
# Vax
#
CFLAGS_vax= $(CDEBUG)
CC_vax=cc
LD_vax=
#
# ------------------------------------------------------------------
# PowerbookG4
#
CFLAGS_ppc7450= $(CDEBUG) -no-cpp-precomp -Wall -funsigned-char -D__bsd
CC_ppc7450= gcc
LD_ppc7450= -lpthread -lm
#
# ------------------------------------------------------------------
# AlphaStation
#
CFLAGS_alpha = $(CDEBUG)
# CC_alpha=cc -w0
#
# A problem with DEC cc -w0 is that there is NO way to suppress
# the damn "Info" messages but keep the "Warning" messages
#
CC_alpha=cc -w1 -std -pthread
# LD_alpha=
LD_alpha= -lpthread -lexc -lm
#
# ------------------------------------------------------------------
# Linux on PC or on AlphaStation (the latter may require some
# special handling since (__linux) and (__alpha) are defined
# 
CFLAGS_linux = $(CDEBUG) -Wall -funsigned-char -D__linux
CC_linux=cc
# LD_linux=
LD_linux= -lpthread -lm
#
# ------------------------------------------------------------------
# Sun has long-standing problems with their fence-sitting
#   System V/BSD posturing (-DBSD_COMP creates MANY more problems
#   than it solves). 
# 
# CFLAGS_sun = $(CDEBUG) -Wall -funsigned-char -D__sunos -DBSD_COMP
CFLAGS_sun = $(CDEBUG) -Wall -funsigned-char -D__sunos 
CC_sun=gcc
LD_sun=-lsocket -lresolv -lnsl -lpthread -lrt -lm
#
# ------------------------------------------------------------------
#
CFLAGS=`cat TMPFLAGS`
CC=`cat TMPCC`
LFLAGS=`cat TMPLD`
#
# The Targets 

default:	tmpfiles 
tmpfiles:	TMPCC

#
# (making TMPCC will also make TMPFLAGS and TMPLD)
TMPCC:		Makefile
		echo "$(CFLAGS_mips)" > TMPFLAGS_mips
		echo "$(CC_mips)" > TMPCC_mips
		echo "$(LD_mips)" > TMPLD_mips
		echo "$(CFLAGS_vax)" > TMPFLAGS_vax
		echo "$(CC_vax)" > TMPCC_vax
		echo "$(LD_vax)" > TMPLD_vax
		echo "$(CFLAGS_alpha)" > TMPFLAGS_alpha
		echo "$(CC_alpha)" > TMPCC_alpha
		echo "$(LD_alpha)" > TMPLD_alpha
		echo "$(CFLAGS_linux)" > TMPFLAGS_
		echo "$(CC_linux)" > TMPCC_
		echo "$(LD_linux)" > TMPLD_
		echo "$(CFLAGS_sun)" > TMPFLAGS_sun
		echo "$(CC_sun)" > TMPCC_sun
		echo "$(LD_sun)" > TMPLD_sun
		echo "$(CFLAGS_ppc7450)" > TMPFLAGS_ppc7450
		echo "$(CC_ppc7450)" > TMPCC_ppc7450
		echo "$(LD_ppc7450)" > TMPLD_ppc7450
		cat TMPFLAGS_$(MACHINE) > TMPFLAGS
		cat TMPCC_$(MACHINE) > TMPCC
		cat TMPLD_$(MACHINE) > TMPLD
		rm -f TMPCC_*
		rm -f TMPLD_*
		rm -f TMPFLAGS_*
#
# -------------------------------------------------------------------
# *** NOTHING needs to be changed above this line ***
# -------------------------------------------------------------------
#
# Ultrix with C2-security requires:
# OPFLAGS= -D_USE_C2
# LFLAGS= -lauth
#
# Old Sparc SunOS 4.1 with usual BSD-security requires:
# OPFLAGS= -D_USE_BSD43
# LFLAGS	= 
#
# -------------------------------------------------------------------
#
#

pipe:		pipesend 
tcp:		vcsend vcrec 
udp:		dgrmsend dgrmrec 
telnet:		s_tlnt s_sh
shell:		s_sh s_daemon
#
# for _bsd
# all:		tcp udp daemon shell pipesend gethost telnet \
#			terminal ngraph

all:		tcp udp daemon shell pipesend process terminal \
			gethost telnet ngraph

#
# -------------------------------------------------------------------
#

process:	TMPCC process.c
	$(CC) $(CFLAGS) process.c -o process $(LFLAGS)

pipesend:	TMPCC pipesend.c \
			header.h startup.h startup.c cleanup.c
	$(CC) $(CFLAGS) pipesend.c -o pipesend $(LFLAGS)
#
# -------------------------------------------------------------------
#

diagnost.o:	TMPCC diagnost.c diagnost.h header.h
	$(CC) -c $(CFLAGS) diagnost.c

g_stdio.o:	TMPCC g_stdio.c g_stdio.h 
	$(CC) -c $(CFLAGS) g_stdio.c

vcsend:		TMPCC vcsend.c diagnost.o \
			header.h startup.h startup.c cleanup.c
	$(CC) $(CFLAGS) vcsend.c diagnost.o -o vcsend $(LFLAGS)

vcrec:		TMPCC vcrec.c diagnost.o \
			header.h startup.h startup.c cleanup.c
	$(CC) $(CFLAGS) vcrec.c diagnost.o -o vcrec $(LFLAGS)

dgrmsend:	TMPCC dgrmsend.c 
	$(CC) $(CFLAGS) dgrmsend.c -o dgrmsend $(LFLAGS)

dgrmrec:	TMPCC dgrmrec.c 
	$(CC) $(CFLAGS) dgrmrec.c -o dgrmrec $(LFLAGS)

daemon:		TMPCC daemon.c diagnost.o \
			header.h startup.h startup.c cleanup.c
	$(CC) $(CFLAGS) daemon.c diagnost.o -o daemon $(LFLAGS)
#
# -------------------------------------------------------------------
#

s_sh:		TMPCC s_sh.c \
			header.h startup.h startup.c cleanup.c
	$(CC) $(CFLAGS) s_sh.c -o s_sh $(LFLAGS)

s_daemon:	TMPCC s_daemon.c diagnost.o \
			header.h startup.h startup.c cleanup.c
	$(CC) $(CFLAGS) s_daemon.c diagnost.o -o s_daemon $(LFLAGS)

s_tlnt:		TMPCC s_tlnt.c \
			header.h startup.h startup.c cleanup.c
	$(CC) $(CFLAGS) s_tlnt.c -o s_tlnt $(LFLAGS)

s_getty:	TMPCC s_getty.c
	$(CC) $(OPFLAGS) $(CFLAGS) s_getty.c $(LFLAGS) -o s_getty $(LFLAGS)
#
# -------------------------------------------------------------------
#

terminal:	TMPCC terminal.c specio.h \
			header.h startup.h startup.c cleanup.c
	$(CC) $(CFLAGS) terminal.c -o terminal $(LFLAGS)

lab3_sol:	TMPCC lab3_sol.c specio.h \
			header.h startup.h startup.c cleanup.c \
			g_stdio.o
	$(CC) $(CFLAGS) lab3_sol.c g_stdio.o -o lab3_sol $(LFLAGS)

lab5_sol:	TMPCC lab5_sol.c diagnost.o \
			header.h startup.h startup.c cleanup.c 
	$(CC) $(CFLAGS) lab5_sol.c diagnost.o -o lab5_sol $(LFLAGS)

ngraph:	TMPCC ngraph.c 
	$(CC) $(CFLAGS) ngraph.c -o ngraph $(LFLAGS)

gethost:	TMPCC gethost.c diagnost.o \
			header.h startup.h startup.c cleanup.c
	$(CC) $(CFLAGS) gethost.c diagnost.o -o gethost $(LFLAGS)
#
# -------------------------------------------------------------------
#
clean:
	rm -f *.o
	rm -f *.log
	rm -f TMP*

veryclean:	clean
	rm -f vcsend vcrec daemon s_daemon s_getty s_tlnt s_sh \
		terminal gethost ngraph pipesend process \
		dgrmsend dgrmrec

