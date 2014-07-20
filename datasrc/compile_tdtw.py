import os, imp, sys
from datatypes import *
import network_tdtw

def create_enum_table(names, num):
	lines = []
	lines += ["enum", "{"]
	lines += ["\t%s=0,"%names[0]]
	for name in names[1:]:
		lines += ["\t%s,"%name]
	lines += ["\t%s" % num, "};"]
	return lines

def create_flags_table(names):
	lines = []
	lines += ["enum", "{"]
	i = 0
	for name in names:
		lines += ["\t%s = 1<<%d," % (name,i)]
		i += 1
	lines += ["};"]
	return lines

def EmitEnum(names, num):
	print("enum")
	print("{")
	print("\t%s=0," % names[0])
	for name in names[1:]:
		print("\t%s," % name)
	print("\t%s" % num)
	print("};")

def EmitFlags(names, num):
	print("enum")
	print("{")
	i = 0
	for name in names:
		print("\t%s = 1<<%d," % (name,i))
		i += 1
	print("};")

gen_tdtw_network_header = False
gen_tdtw_network_source = False

if "network_tdtw_header" in sys.argv: gen_tdtw_network_header = True
if "network_tdtw_source" in sys.argv: gen_tdtw_network_source = True


# NETWORK
if gen_tdtw_network_header:

	print("#ifndef GAME_GENERATED_PROTOCOL_TDTW_H")
	print("#define GAME_GENERATED_PROTOCOL_TDTW_H")
	print(network_tdtw.RawHeader)

	for e in network_tdtw.Flags:
		for l in create_flags_table(["%s_%s" % (e.name, v) for v in e.values]): print(l)
		print("")

	for l in create_enum_table(["NETMSGTYPE_TDTW_INVALID"]+[o.enum_name for o in network_tdtw.Messages], "NUM_NETMSGTYPES_TDTW"): print(l)
	print("")

	for item in network_tdtw.Messages:
		for line in item.emit_declaration():
			print(line)
		print("")

	print("""

class CNetObjHandlerTdtw
{
protected:
	const char *m_pMsgFailedOn;
	char m_aMsgData[1024];
private:
	static const char *ms_apMsgNames[];

public:
	CNetObjHandlerTdtw();

	const char *GetMsgName(int Type);
	void *SecureUnpackMsg(int Type, CUnpacker *pUnpacker);
	const char *FailedMsgOn();
};

""")

	print("#endif // GAME_GENERATED_PROTOCOL_H")


if gen_tdtw_network_source:
	# create names
	lines = []

	lines += ['#include <engine/shared/protocol.h>']
	lines += ['#include <engine/message.h>']
	lines += ['#include "protocol_tdtw.h"']

	lines += ['CNetObjHandlerTdtw::CNetObjHandlerTdtw()']
	lines += ['{']
	lines += ['\tm_pMsgFailedOn = "";']
	lines += ['}']
	lines += ['']
	lines += ['const char *CNetObjHandlerTdtw::FailedMsgOn() { return m_pMsgFailedOn; }']
	lines += ['']
	lines += ['']
	lines += ['']
	lines += ['']
	lines += ['']

	lines += ['static const int max_int = 0x7fffffff;']

	lines += ['const char *CNetObjHandlerTdtw::ms_apMsgNames[] = {']
	lines += ['\t"invalid",']
	for msg in network_tdtw.Messages:
		lines += ['\t"%s",' % msg.name]
	lines += ['\t""']
	lines += ['};']
	lines += ['']

	lines += ['const char *CNetObjHandlerTdtw::GetMsgName(int Type)']
	lines += ['{']
	lines += ['\tif(Type < 0 || Type >= NUM_NETMSGTYPES_TDTW) return "(out of range)";']
	lines += ['\treturn ms_apMsgNames[Type];']
	lines += ['};']
	lines += ['']


	for l in lines:
		print(l)
	lines = []
	if 0:
		for item in network_tdtw.Messages:
			for line in item.emit_unpack():
				print(line)
			print("")

		lines += ['static void *secure_unpack_invalid(CUnpacker *pUnpacker) { return 0; }']
		lines += ['typedef void *(*SECUREUNPACKFUNC)(CUnpacker *pUnpacker);']
		lines += ['static SECUREUNPACKFUNC secure_unpack_funcs[] = {']
		lines += ['\tsecure_unpack_invalid,']
		for msg in network_tdtw.Messages:
			lines += ['\tsecure_unpack_%s,' % msg.name]
		lines += ['\t0x0']
		lines += ['};']

	#
	lines += ['void *CNetObjHandlerTdtw::SecureUnpackMsg(int Type, CUnpacker *pUnpacker)']
	lines += ['{']
	lines += ['\tm_pMsgFailedOn = 0;']
	lines += ['\tswitch(Type)']
	lines += ['\t{']


	for item in network_tdtw.Messages:
		for line in item.emit_unpack():
			lines += ["\t" + line]
		lines += ['\t']

	lines += ['\tdefault:']
	lines += ['\t\tm_pMsgFailedOn = "(type out of range)";']
	lines += ['\t\tbreak;']
	lines += ['\t}']
	lines += ['\t']
	lines += ['\tif(pUnpacker->Error())']
	lines += ['\t\tm_pMsgFailedOn = "(unpack error)";']
	lines += ['\t']
	lines += ['\tif(m_pMsgFailedOn)']
	lines += ['\t\treturn 0;']
	lines += ['\tm_pMsgFailedOn = "";']
	lines += ['\treturn m_aMsgData;']
	lines += ['};']
	lines += ['']


	for l in lines:
		print(l)
