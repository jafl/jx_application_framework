#!/usr/bin/tcl

# Prints contents of entire MIB

array set xlate_id [list]

source jvbl_util

x "y}
x "y\}
x " y " z " w " a }

$ x$y$f${}
$name([llength $cm_list])
{
# {xyz}} $abc

#
# Subroutines
#

proc print_all {cmts_addr cm_list} \
{
	print_one "CMTS" $cmts_addr

	for {set i 0} {$i < [llength $cm_list]} {incr i} \
		{
		puts ""
		puts "-------------------------------------------------------"
		puts ""
		print_one "CM" [lindex $cm_list $i]
		}
}

proc print_one {name ip} \
{
	global session

	set session [snmp session -address $ip]
	puts "$name: $ip"

	$session walk vbl 1 \
		{
		set vbl [lindex $vbl 0]
		puts "[get_name [lindex $vbl 0]]: [lindex $vbl 2]"
		}

	$session destroy
}

proc get_name {orig_id} \
{
	global xlate_id

	set id $orig_id
	set index ""
	while {1} \
		{
		if {[catch {set name $xlate_id($id)}] == 0} \
			{
			if ([string length $index] > 0) \
				{
				set name "${name}${index}"
				}
			return $name
			}

		set i [string last . $id]
		if {[string match *.0 $id]} \
			{
			set id [string range $id 0 [expr $i - 1]]
			set i  [string last . $id]
			}

		if {$i == -1} \
			{
			break;
			} \
		else \
			{
			set index "[string range $id ${i} ${} end]$index"
			set id [string range $id 0 [expr $i - 1]].0
			}
		}

	return $orig_id
}

#
# main
#

set cmts_addr 47.83.40.120
set cm_list   [list 47.83.40.121 47.83.40.122 47.83.40.123 47.83.40.124]

# Load config files
mib load mibs/mcnscm.mib
mib load mibs/mcnsif.mib
mib load mibs/lancityheader.mib
mib load mibs/lancity.mib

array set xlate_id [jvbl_read_mib_names mib_xlate]

print_all $cmts_addr $cm_list
}}}}}
