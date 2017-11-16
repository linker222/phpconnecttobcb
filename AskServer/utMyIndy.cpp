//---------------------------------------------------------------------------

#include <vcl.h>
#include <stdio.h>
#include <IdGlobal.hpp>
#include <idsync.hpp>
#include <System.JSON.hpp>
#include <IdHashMessageDigest.hpp>
#pragma hdrstop

#include "utMyIndy.h"
//---------------------------------------------------------------------------
#pragma package(smart_init)
#pragma resource "*.dfm"
TfmMyIndy *fmMyIndy;
//---------------------------------------------------------------------------
class AddToMemoNotify : public TIdNotify
{
protected:
	TfmMyIndy *m_Adapter;
	String m_Msg;

    virtual void __fastcall DoNotify()
	{
		m_Adapter->AddMemoString(m_Msg);
    }
public:
	__fastcall AddToMemoNotify(TfmMyIndy *Adapter, const String &Msg):TIdNotify(),m_Adapter(Adapter),m_Msg(Msg)
    {
    }
};
//---------------------------------------------------------------------------
class TMemoAddNotify : public TIdNotify
{
private:
	String FMsg;
	__fastcall TMemoAddNotify(const String &AMsg):TIdNotify(), FMsg(AMsg){}
protected:
	void __fastcall DoNotify()
	{
		fmMyIndy->Memo_Server->Lines->Add(FMsg);
	}
public:
	static void __fastcall AddToMemo(const String &AMsg)
	{
		Word Year, Month, Day, Hour, Min, Sec, MSec;
		TDateTime dtPresent = Now();
		DecodeDate(dtPresent, Year, Month, Day);
		DecodeTime(dtPresent, Hour, Min, Sec, MSec);
		if ( fmMyIndy->Memo_Server->Lines->Count >= 1000 )
		{
			fmMyIndy->Memo_Server->Lines->Clear();
		}
		char OutDateTime[48];
		sprintf( OutDateTime, "  %4d/%02d/%02d  -  %02d:%02d:%02d  ",Year, Month, Day, Hour, Min, Sec );
		(new TMemoAddNotify(String(OutDateTime) + "  " + AMsg))->Notify();
	}
};
//---------------------------------------------------------------------------
enum COMMAND_TYPE
{
	_MSG = 100,
	_STRING = 101,
	_GETIP = 102
};
//---------------------------------------------------------------------------
struct COMMAND_MSG_INFO
{
	COMMAND_TYPE CommandType;
	WideChar  Msg1[1024];
	WideChar  Msg2[1024];
};
//---------------------------------------------------------------------------
struct COMMAND_INFO
{
	COMMAND_TYPE CommandType;
};
//---------------------------------------------------------------------------
struct TConnectedClient
{
  //TDateTime Connect;
  //TDateTime LastAction;
  unsigned int TimeOut;
  String IP;
  TADOQuery *ADO;
  TIdContext *AContext;
  //unsigned int ThreadID;
};
//---------------------------------------------------------------------------
__fastcall TfmMyIndy::TfmMyIndy(TComponent* Owner)
	: TForm(Owner)
{
}
//---------------------------------------------------------------------------
String __fastcall TfmMyIndy::GetHostIpAddress(void)
{
	struct hostent *thisHost;
	struct in_addr in;
	char MyName[80];
	char *ptr,*hostname;
	WORD wVersionRequested;
	WSADATA wsaData;
	int err;
	wVersionRequested = MAKEWORD(2,0);

	err = WSAStartup(wVersionRequested,&wsaData);
	if ( err!=0 ) return "";
	if ( LOBYTE(wsaData.wVersion)!=2 || HIBYTE(wsaData.wVersion) != 0 )
	{
		WSACleanup();
		return "";
	}
	if ( gethostname(MyName,80) == SOCKET_ERROR ) return "";
	if ( !( thisHost=gethostbyname(MyName) ) ) return "";
	hostname = thisHost->h_name;
	memset((void*)&in,sizeof(in),0);
	in.s_addr = *((unsigned long*)thisHost->h_addr_list[0]);
	if (!(ptr = inet_ntoa(in))) return "";
	WSACleanup();
	return (String)ptr;
}
//---------------------------------------------------------------------------
void __fastcall TfmMyIndy::AddMemoString ( String iStr )
{
	Word Year, Month, Day, Hour, Min, Sec, MSec;
	TDateTime dtPresent = Now();
	DecodeDate(dtPresent, Year, Month, Day);
	DecodeTime(dtPresent, Hour, Min, Sec, MSec);
	if ( Memo_Server->Lines->Count >= 1000 )
	{
		Memo_Server->Lines->Clear();
	}
	char OutDateTime[48];
	sprintf( OutDateTime, "  %4d/%02d/%02d  -  %02d:%02d:%02d  ",Year, Month, Day, Hour, Min, Sec );
	Memo_Server->Lines->Append( String(OutDateTime) + "  " + (iStr) );
}
//---------------------------------------------------------------------------
void __fastcall TfmMyIndy::RefreshListDisplay()
{
	TIdContext *AContext;
	try
	{
		TList *ContextList = IdTCPServer1->Contexts->LockList();
		Label_ConnectCount->Caption = IntToStr(ContextList->Count);
		for ( int i=0 ; i< ContextList->Count ; i++)
		{
			AContext = (TIdContext *)ContextList->Items[i];
			try
			{
				if ( AContext == NULL ) continue;
				TConnectedClient *MyClient = (TConnectedClient*) AContext->Data;
				AddMemoString(MyClient->IP);
			}
			catch(...)
			{
				AddMemoString( "RefreshListDisplay error" );
			}
		}
	}
	__finally
	{
		IdTCPServer1->Contexts->UnlockList();
	}
}
//---------------------------------------------------------------------------
void __fastcall TfmMyIndy::FreeConnectedClient(TIdContext *AContext)
{
	try
	{
		if ( AContext->Data )
		{
			if ( AContext->Connection->Connected() )
			{
				CloseConnect(AContext);
			}
			TConnectedClient *MyClient = reinterpret_cast<TConnectedClient*>(AContext->Data);
			if ( MyClient != NULL )
			{
				if ( MyClient->ADO != NULL )
				{
					MyClient->ADO->Close();
					MyClient->ADO->ConnectionString = "";
					delete MyClient->ADO;
					MyClient->ADO = NULL;
				}
				delete MyClient;
				MyClient = NULL;
			}
			AContext->Data = NULL;
			delete AContext->Data;
		}
	}
	catch(...)
	{
		CloseConnect(AContext);
		AddMemoString("Free TConnectedClient error");
	}
}
//---------------------------------------------------------------------------
String __fastcall TfmMyIndy::SendClientIp(String iSIP)
{
	TIdContext *AContext;
	try
	{
		TList *ContextList = IdTCPServer1->Contexts->LockList();
		Label_ConnectCount->Caption = IntToStr(ContextList->Count);
		String iIP = "";
		for ( int i=0 ; i< ContextList->Count ; i++)
		{
			AContext = (TIdContext *)ContextList->Items[i];
			try
			{
				if ( AContext == NULL ) continue;
				TConnectedClient *MyClient = (TConnectedClient*) AContext->Data;
				if ( iSIP != MyClient->IP )
				{
					iIP = MyClient->IP;
					break;
				}
			}
			catch(...)
			{
				AddMemoString( "SendClientIp error" );
			}
		}
		return iIP;
	}
	__finally
	{
		IdTCPServer1->Contexts->UnlockList();
	}
}
//---------------------------------------------------------------------------
void __fastcall TfmMyIndy::IncConnectionCount()
{
	FConnectionCount++;
	Label_ConnectCount->Caption = IntToStr(FConnectionCount);
}
//---------------------------------------------------------------------------

void __fastcall TfmMyIndy::DecConnectionCount()
{
	FConnectionCount--;
	Label_ConnectCount->Caption = IntToStr(FConnectionCount);
}
//---------------------------------------------------------------------------
void __fastcall TfmMyIndy::CloseConnect(TIdContext *AContext)
{
	if ( AContext->Connection->IOHandler->InputBuffer->Size )
		AContext->Connection->IOHandler->InputBuffer->Clear();
	AContext->Connection->IOHandler->CloseGracefully();
	AContext->Connection->Disconnect();
	TIdSync::SynchronizeMethod(&DecConnectionCount);
}
//---------------------------------------------------------------------------
String __fastcall TfmMyIndy::HashString(String iStr)
{
	TIdHashMessageDigest5 *md5;
	try
	{
		md5 = new TIdHashMessageDigest5();
		String hash = md5->HashStringAsHex(iStr, IndyTextEncoding(TEncoding::ASCII)).LowerCase();
		return (hash);
	}
	__finally
	{
		delete md5;
	}
}
//---------------------------------------------------------------------------
int TfmMyIndy::mix(char m, int s)
{
	return ((s<<7) + ~(s>>3)) + ~m;
}
int TfmMyIndy::hash(const char * m)
{
	return (*m) ? mix(*m, hash(m+1)) : 0;
}
//---------------------------------------------------------------------------
void __fastcall TfmMyIndy::Button_StartClick(TObject *Sender)
{
	IdTCPServer1->Active = false;

	IdTCPServer1->Bindings->Clear();
	IdTCPServer1->Bindings->Add();
	IdTCPServer1->Bindings->Items[0]->IP = Edit_IP->Text;
	IdTCPServer1->Bindings->Items[0]->Port = StrToInt(Edit_Port->Text);
	IdTCPServer1->ListenQueue = 15;
	IdTCPServer1->MaxConnections = 10;
	IdTCPServer1->Active = true;
	if (IdTCPServer1->Active)
	{
		AddMemoString( " Server Running on " + IdTCPServer1->Bindings->Items[0]->IP + ":" + IntToStr(IdTCPServer1->Bindings->Items[0]->Port));
	}

}
//---------------------------------------------------------------------------
void __fastcall TfmMyIndy::IdTCPServer1Connect(TIdContext *AContext)
{
	CoInitialize(NULL);
	TADOQuery *ADO = new TADOQuery(NULL);
	TConnectedClient *NewClient = new TConnectedClient();
	try
	{
		ADO->ConnectionString = ConnectStr;
		NewClient->TimeOut = 0;
		/*
		try
		{
			AddMemoString(GStack->HostByAddress(AContext->Binding->PeerIP, AContext->Binding->IPVersion));  //We do things this way because we are likely have a reverse DNS failure if you have a computer with IP address and no DNS name at all.
		}
		catch (Exception &e)
		{

		}
		*/
		NewClient->IP = AContext->Connection->Socket->Binding->PeerIP;
		NewClient->ADO = ADO;
		NewClient->AContext = AContext;
		AContext->Data = reinterpret_cast<TObject*>(NewClient);
		(new AddToMemoNotify(fmMyIndy, NewClient->IP + "連線"))->Notify();
		//AddMemoString( NewClient->IP + "連線" );
	}
	catch (Exception &e)
	{
		delete ADO;
		delete NewClient;
	}
}
//---------------------------------------------------------------------------
void __fastcall TfmMyIndy::IdTCPServer1Disconnect(TIdContext *AContext)
{
	AddMemoString( AContext->Connection->Socket->Binding->PeerIP + "離線" );
	FreeConnectedClient(AContext);
	CoUninitialize();
}
//---------------------------------------------------------------------------
void __fastcall TfmMyIndy::IdTCPServer1Exception(TIdContext *AContext, Exception *AException)
{
	FreeConnectedClient( AContext );
}
//---------------------------------------------------------------------------
void __fastcall TfmMyIndy::IdTCPServer1Status(TObject *ASender, const TIdStatus AStatus, const UnicodeString AStatusText)

{
	AddMemoString("Status : " + AStatusText);
}
//---------------------------------------------------------------------------
void __fastcall TfmMyIndy::IdTCPServer1Execute(TIdContext *AContext)
{
	TBytes buffer;
	IndySleep(10);
	TIdIOHandler *Hb;

	try
	{
		try
		{
			TIdSync::SynchronizeMethod(&IncConnectionCount);
			Hb = AContext->Connection->IOHandler;
			if (Hb->InputBufferIsEmpty())
			{
				Hb->DefStringEncoding = enUTF8;
				Hb->CheckForDisconnect(true, true);
				AContext->Connection->CheckForGracefulDisconnect(true);
				Hb->ReadBytes(buffer, -1, false);
				String read_buffer = BytesToString(buffer);
				AddMemoString(read_buffer);
				//TMemoAddNotify::AddToMemo(read_buffer);
				TJSONObject *o = (TJSONObject*) TJSONObject::ParseJSONValue(TEncoding::ASCII->GetBytes(read_buffer), 0);
				if (o == NULL)
				{
					//CloseConnect(AContext);
					return;
				}
				try
				{
					TJSONPair *jMode = o->Get("mode");
					if ( jMode == NULL )
					{
						//CloseConnect(AContext);
						return;
					}
					String mode = jMode->JsonValue->Value();
					if ( mode == "sendquestion" )
					{
						TMemoAddNotify::AddToMemo(((TJSONPair*)o->Get("name"))->JsonValue->Value() + " 先生/小姐");
						SendAnswer(Hb, ((TJSONPair*)o->Get("name"))->JsonValue->Value());
					}
					else
					{
					}
				}
				__finally
				{
					o->Free();
				}
			}
		}
		__finally
		{
			CloseConnect(AContext);
		}
	}
	catch (Exception &e)
	{
		CloseConnect(AContext);
	}
	/*
	String Message;
	COMMAND_INFO send_command_pack;
	TBytes buffer;
	IndySleep(10);
	try
	{
		try
		{
			//TIdSync::SynchronizeMethod(&IncConnectionCount);
			AContext->Connection->IOHandler->CheckForDisconnect(true, true);
			memset(&send_command_pack, 0, sizeof(COMMAND_INFO));
			AContext->Connection->IOHandler->ReadBytes(buffer, sizeof(COMMAND_INFO), false);
			BytesToRaw(buffer, &send_command_pack, sizeof(COMMAND_INFO));
			switch ( send_command_pack.CommandType )
			{
				case _MSG :
				{
					COMMAND_MSG_INFO send_pack;
					memset(&send_pack, 0, sizeof(COMMAND_MSG_INFO));
					AContext->Connection->IOHandler->ReadBytes(buffer, sizeof(COMMAND_MSG_INFO), false);
					BytesToRaw(buffer, &send_pack, sizeof(COMMAND_MSG_INFO));
					AddMemoString( AContext->Connection->Socket->Binding->PeerIP + " 送出訊息：" + send_pack.Msg2 );
					String SnedStr = send_pack.Msg2;
					AContext->Connection->IOHandler->WriteLn( SnedStr, enUTF8 );
					break;
				}
				case _GETIP :
				{
					AddMemoString(AContext->Connection->Socket->Binding->PeerIP + " GET IP" );
					AContext->Connection->IOHandler->WriteLn( SendClientIp(AContext->Connection->Socket->Binding->PeerIP), enUTF8);
					break;
				}

				case _LOGIN :
				{
					AddMemoString(AContext->Connection->Socket->Binding->PeerIP + "：" + String(send_command_pack.User) + " - Login" );
					UpdateLoginInfo( AContext, String(send_command_pack.Area), String(send_command_pack.User) );
					break;
				}
				case _UPLOADFILEBYPACKET :
				{
					AddMemoString(AContext->Connection->Socket->Binding->PeerIP + "：" + String(send_command_pack.User) + " - UploadFile" );
					UploadFileInfo ( AContext, String(send_command_pack.Area), String(send_command_pack.User) );
					break;
				}
				case _DOWNLOADFILE :
				{
					AddMemoString(AContext->Connection->Socket->Binding->PeerIP + "：" + String(send_command_pack.User) + " - DownloadFile" );
					DownLoadFileInfo ( AContext, String(send_command_pack.Area), String(send_command_pack.User) );
					break;
				}
				case _UPLOADSTUDENTBYPACKET :
				{
					AddMemoString(AContext->Connection->Socket->Binding->PeerIP + "：" + String(send_command_pack.User) + " - UploadStudentByPacket" );
					UploadStudentByPacket ( AContext, String(send_command_pack.Area), String(send_command_pack.User) );
					break;
				}
				case _UPLOADSERVICEBYPACKET :
				{
					AddMemoString(AContext->Connection->Socket->Binding->PeerIP + "：" + String(send_command_pack.User) + " - UploadServiceByPacket" );

					UploadServiceByPacket ( AContext, String(send_command_pack.Area), String(send_command_pack.User) );
					break;
				}
				case _UPLOADSTUDENTROSTERBYPACKET :
				{
					AddMemoString(AContext->Connection->Socket->Binding->PeerIP + "：" + String(send_command_pack.User) + " - UploadStudentRosterByPacket" );
					UploadStudentRosterByPacket ( AContext, String(send_command_pack.Area), String(send_command_pack.User) );
					break;
				}
				case _DOWNLOADSTUDENT :
				{
					AddMemoString(AContext->Connection->Socket->Binding->PeerIP + "：" + String(send_command_pack.User) + " - _DOWNLOADSTUDENT" );
					DownloadStudent( AContext, String(send_command_pack.Area), String(send_command_pack.User) );
					break;
				}
				case _DOWNLOADSERVICE :
				{
					AddMemoString(AContext->Connection->Socket->Binding->PeerIP + "：" + String(send_command_pack.User) + " - _DOWNLOADSERVICE" );
					DownloadService( AContext, String(send_command_pack.Area), String(send_command_pack.User) );
					break;
				}
				case _DOWNLOADSTUDENTROSTER :
				{
					AddMemoString(AContext->Connection->Socket->Binding->PeerIP + "：" + String(send_command_pack.User) + " - _DOWNLOADSTUDENTROSTER" );
					DownloadStudentRoster( AContext, String(send_command_pack.Area), String(send_command_pack.User) );
					break;
				}
				case _GETSCHOOLUPLOADSTATUS :
				{
					AddMemoString(AContext->Connection->Socket->Binding->PeerIP + "：" + String(send_command_pack.User) + " - _GETSCHOOLUPLOADSTATUS" );
					GetSchoolUploadStatus( AContext, String(send_command_pack.Area), String(send_command_pack.User) );
					break;
				}

				default:
				{
					AddMemoString(AContext->Connection->Socket->Binding->PeerIP + "：unknow" );
					if ( AContext->Connection->IOHandler->InputBuffer->Size )
						AContext->Connection->IOHandler->InputBuffer->Clear();\
					AContext->Connection->IOHandler->CloseGracefully();
					AContext->Connection->Disconnect();
					//TIdSync::SynchronizeMethod(&DecConnectionCount);
					break;
				}
			}
		}
		__finally
		{
			if ( AContext->Connection->IOHandler->InputBuffer->Size )
				AContext->Connection->IOHandler->InputBuffer->Clear();\
			AContext->Connection->IOHandler->CloseGracefully();
			AContext->Connection->Disconnect();
			//FreeConnectedClient(AContext);
			//TIdSync::SynchronizeMethod(&DecConnectionCount);
		}
	}
	catch (Exception &e)
	{
		if ( AContext->Connection->IOHandler->InputBuffer->Size )
				AContext->Connection->IOHandler->InputBuffer->Clear();\
		AContext->Connection->IOHandler->CloseGracefully();
		AContext->Connection->Disconnect();
		//FreeConnectedClient(AContext);
		//TIdSync::SynchronizeMethod(&DecConnectionCount);
	}
	*/
}
//---------------------------------------------------------------------------
void __fastcall TfmMyIndy::FormDestroy(TObject *Sender)
{
	TIdContext *AContext;
	try
	{
		TList *ContextList = IdTCPServer1->Contexts->LockList();
		for ( int i=0 ; i< ContextList->Count ; i++)
		{
			AContext = (TIdContext *)ContextList->Items[i];
			if ( AContext == NULL ) continue;
			if ( AContext->Connection->Connected() )
			{
				AContext->Connection->Disconnect();
			}
		}
	}
	__finally
	{
		IdTCPServer1->Contexts->UnlockList();
	}
	IdTCPServer1->Active = false;
}
//---------------------------------------------------------------------------
void __fastcall TfmMyIndy::Timer1Timer(TObject *Sender)
{
	TIdContext *AContext;
	try
	{
		TList *ContextList = IdTCPServer1->Contexts->LockList();
		Label_ConnectCount->Caption = IntToStr(ContextList->Count);
		for ( int i=0 ; i< ContextList->Count ; i++)
		{
			AContext = (TIdContext *)ContextList->Items[i];
			try
			{
				if ( AContext == NULL ) continue;
				TConnectedClient *MyClient = (TConnectedClient*) AContext->Data;

				if ( MyClient->TimeOut > 60 )
				{
					AddMemoString( MyClient->IP + " TimeOut" );
					if ( AContext->Connection->Connected() )
					{
						//AContext->Connection->Disconnect();
						CloseConnect(AContext);
					}
				}
				else
				{
					MyClient->TimeOut = MyClient->TimeOut + 1;
				}

			}
			catch(...)
			{
				AddMemoString( "Timer1Timer error" );
			}
		}
    }
    __finally
    {
		IdTCPServer1->Contexts->UnlockList();
	}
}
//---------------------------------------------------------------------------

void __fastcall TfmMyIndy::Button_CountConnectClick(TObject *Sender)
{
	RefreshListDisplay();
}
//---------------------------------------------------------------------------

void __fastcall TfmMyIndy::FormShow(TObject *Sender)
{
	Edit_IP->Text = GetHostIpAddress();
	AddMemoString( "Server 尚未啟動");
	Button_StartClick(Sender);
	/*
	ExePath = ExtractFilePath( Application->ExeName );
	if ( FileExists( ExePath + "cap.udl" ) )
	{
		ConnectStr = "File Name=" + ExePath + "cap.udl";
	}
	else
	{
		ShowMessage("遺失資料庫設定檔cap.udl，無法連線至資料庫！");
		return;
	}
	*/
}
//---------------------------------------------------------------------------

void __fastcall TfmMyIndy::FormCreate(TObject *Sender)
{
	FConnectionCount = 0;
}
//---------------------------------------------------------------------------
void __fastcall TfmMyIndy::SendAnswer(TIdIOHandler *AIOHandler, String iQuestion)
{
	TBytes buffer;
	TJSONObject *tomsg = new TJSONObject();
	try
	{
		//AIOHandler->ReadBytes(buffer, -1, false);
		//String read_buffer = BytesToString(buffer);
		//AddMemoString(read_buffer);
		tomsg->AddPair(new TJSONPair("title", "題目"));
		tomsg->AddPair(new TJSONPair("subtitle", "副題"));
		tomsg->AddPair(new TJSONPair("author", iQuestion + " 先生/小姐"));
		AIOHandler->Write(tomsg->ToString(), enUTF8);
		/*
			String SnedStr = "施宏政";
			AIOHandler->WriteLn( SnedStr, enUTF8 );
		*/
	}
	__finally
	{
		tomsg->Free();
	}
}
//---------------------------------------------------------------------------
