#pragma once

Const2Str_st gar_Winerr2Str[] = 
{
	ITEM_Const2Str(ERROR_SUCCESS),
	ITEM_Const2Str(ERROR_FILE_NOT_FOUND), // 2
	ITEM_Const2Str(ERROR_ACCESS_DENIED), // 5
	ITEM_Const2Str(ERROR_BAD_NETPATH),   // 53, client, server not reachable
	ITEM_Const2Str(ERROR_NETNAME_DELETED), // 64, client, broken pipe detected after FlushFileBuffers()
	ITEM_Const2Str(ERROR_INVALID_PARAMETER), // 87
	ITEM_Const2Str(ERROR_BROKEN_PIPE), // 109, server
	ITEM_Const2Str(ERROR_SEM_TIMEOUT), // 121
	ITEM_Const2Str(ERROR_BAD_PATHNAME), // 161
//	ITEM_Const2Str(),
	ITEM_Const2Str(ERROR_PIPE_BUSY), // 231, server & client
	ITEM_Const2Str(ERROR_NO_DATA), // 232, WriteFile() to broken pipe
	ITEM_Const2Str(ERROR_PIPE_CONNECTED), // 535, server, actually ConnectNamedPipe() success
	ITEM_Const2Str(ERROR_PIPE_LISTENING), // 536, server
	ITEM_Const2Str(ERROR_OPERATION_ABORTED), // 995
	ITEM_Const2Str(ERROR_IO_PENDING), // 997
	ITEM_Const2Str(ERROR_NOACCESS), // 998
	ITEM_Const2Str(ERROR_LOGON_FAILURE), // 1326, unknown user name or bad password
	ITEM_Const2Str(ERROR_CANNOT_IMPERSONATE), // 1368

//	ITEM_Const2Str(),
};

