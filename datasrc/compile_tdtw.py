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

	for l in create_enum_table(["NETOBJTYPE_TDTW_INVALID"]+[o.enum_name for o in network_tdtw.Objects], "NUM_NETOBJTYPES_TDTW"): print(l)
	print("")
	for l in create_enum_table(["NETMSGTYPE_TDTW_INVALID"]+[o.enum_name for o in network_tdtw.Messages], "NUM_NETMSGTYPES_TDTW"): print(l)
	print("")

	for item in network_tdtw.Objects + network_tdtw.Messages:
		for line in item.emit_declaration():
			print(line)
		print("")

	print("""

class CNetObjHandlerTdtw
{
protected:
	const char *m_pMsgFailedOn;
	const char *m_pObjCorrectedOn;
	char m_aMsgData[1024];
	int m_NumObjCorrections;
	int ClampInt(const char *pErrorMsg, int Value, int Min, int Max);
private:
	static const char *ms_apObjNames[];
	static int ms_aObjSizes[];
	static const char *ms_apMsgNames[];

public:
	CNetObjHandlerTdtw();

	int ValidateObj(int Type, void *pData, int Size);
	const char *GetObjName(int Type);
	int GetObjSize(int Type);
	int NumObjCorrections();
	const char *CorrectedObjOn();

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
	lines += ['\tm_pObjCorrectedOn = "";']
	lines += ['\tm_NumObjCorrections = 0;']
	lines += ['}']
	lines += ['']
	lines += ['int CNetObjHandlerTdtw::NumObjCorrections() { return m_NumObjCorrections; }']
	lines += ['const char *CNetObjHandlerTdtw::CorrectedObjOn() { return m_pObjCorrectedOn; }']
	lines += ['const char *CNetObjHandlerTdtw::FailedMsgOn() { return m_pMsgFailedOn; }']
	lines += ['']
	lines += ['']
	lines += ['']
	lines += ['']
	lines += ['']

	lines += ['static const int max_int = 0x7fffffff;']

	lines += ['int CNetObjHandlerTdtw::ClampInt(const char *pErrorMsg, int Value, int Min, int Max)']
	lines += ['{']
	lines += ['\tif(Value < Min) { m_pObjCorrectedOn = pErrorMsg; m_NumObjCorrections++; return Min; }']
	lines += ['\tif(Value > Max) { m_pObjCorrectedOn = pErrorMsg; m_NumObjCorrections++; return Max; }']
	lines += ['\treturn Value;']
	lines += ['}']

	lines += ["const char *CNetObjHandlerTdtw::ms_apObjNames[] = {"]
	lines += ['\t"invalid",']
	lines += ['\t"%s",' % o.name for o in network_tdtw.Objects]
	lines += ['\t""', "};", ""]

	lines += ["int CNetObjHandlerTdtw::ms_aObjSizes[] = {"]
	lines += ['\t0,']
	lines += ['\tsizeof(%s),' % o.struct_name for o in network_tdtw.Objects]
	lines += ['\t0', "};", ""]


	lines += ['const char *CNetObjHandlerTdtw::ms_apMsgNames[] = {']
	lines += ['\t"invalid",']
	for msg in network_tdtw.Messages:
		lines += ['\t"%s",' % msg.name]
	lines += ['\t""']
	lines += ['};']
	lines += ['']

	lines += ['const char *CNetObjHandlerTdtw::GetObjName(int Type)']
	lines += ['{']
	lines += ['\tif(Type < 0 || Type >= NUM_NETOBJTYPES_TDTW) return "(out of range)";']
	lines += ['\treturn ms_apObjNames[Type];']
	lines += ['};']
	lines += ['']

	lines += ['int CNetObjHandlerTdtw::GetObjSize(int Type)']
	lines += ['{']
	lines += ['\tif(Type < 0 || Type >= NUM_NETOBJTYPES_TDTW) return 0;']
	lines += ['\treturn ms_aObjSizes[Type];']
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

	if 0:
		for item in network_tdtw.Objects:
			for line in item.emit_validate():
				print(line)
			print("")

	# create validate tables
		lines = []
		lines += ['static int validate_invalid(void *data, int size) { return -1; }']
		lines += ["typedef int(*VALIDATEFUNC)(void *data, int size);"]
		lines += ["static VALIDATEFUNC validate_funcs[] = {"]
		lines += ['\tvalidate_invalid,']
		lines += ['\tvalidate_%s,' % o.name for o in network_tdtw.Objects]
		lines += ["\t0x0", "};", ""]

		lines += ["int netobj_validate(int type, void *data, int size)"]
		lines += ["{"]
		lines += ["\tif(type < 0 || type >= NUM_NETOBJTYPES_TDTW) return -1;"]
		lines += ["\treturn validate_funcs[type](data, size);"]
		lines += ["};", ""]

	lines = []
	lines += ['int CNetObjHandlerTdtw::ValidateObj(int Type, void *pData, int Size)']
	lines += ['{']
	lines += ['\tswitch(Type)']
	lines += ['\t{']

	for item in network_tdtw.Objects:
		for line in item.emit_validate():
			lines += ["\t" + line]
		lines += ['\t']
	lines += ['\t}']
	lines += ['\treturn -1;']
	lines += ['};']
	lines += ['']

#int Validate(int Type, void *pData, int Size);

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
