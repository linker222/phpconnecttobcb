//---------------------------------------------------------------------------

#ifndef utMyIndyH
#define utMyIndyH
//---------------------------------------------------------------------------
#include <System.Classes.hpp>
#include <Vcl.Controls.hpp>
#include <Vcl.StdCtrls.hpp>
#include <Vcl.Forms.hpp>
#include <IdBaseComponent.hpp>
#include <IdComponent.hpp>
#include <IdContext.hpp>
#include <IdCustomTCPServer.hpp>
#include <IdTCPServer.hpp>
#include <Vcl.ComCtrls.hpp>
#include <Vcl.ExtCtrls.hpp>
#include <Data.DB.hpp>
#include <Data.Win.ADODB.hpp>
#include <IdTCPClient.hpp>
#include <IdTCPConnection.hpp>
#include <System.SysUtils.hpp>
//---------------------------------------------------------------------------
class TfmMyIndy : public TForm
{
__published:	// IDE-managed Components
	TIdTCPServer *IdTCPServer1;
	TPanel *Panel1;
	TPanel *Panel2;
	TLabel *Label1;
	TLabel *Label_ConnectCount;
	TLabel *Label3;
	TEdit *Edit_Port;
	TButton *Button_Start;
	TTimer *Timer1;
	TLabel *Label4;
	TEdit *Edit_IP;
	TButton *Button_CountConnect;
	TMemo *Memo_Server;
	void __fastcall Button_StartClick(TObject *Sender);
	void __fastcall IdTCPServer1Connect(TIdContext *AContext);
	void __fastcall IdTCPServer1Disconnect(TIdContext *AContext);
	void __fastcall IdTCPServer1Exception(TIdContext *AContext, Exception *AException);
	void __fastcall IdTCPServer1Status(TObject *ASender, const TIdStatus AStatus, const UnicodeString AStatusText);
	void __fastcall IdTCPServer1Execute(TIdContext *AContext);
	void __fastcall FormDestroy(TObject *Sender);
	void __fastcall Timer1Timer(TObject *Sender);
	void __fastcall Button_CountConnectClick(TObject *Sender);
	void __fastcall FormShow(TObject *Sender);
	void __fastcall FormCreate(TObject *Sender);

private:	// User declarations
	String ExePath;
	String ConnectStr;
	int FConnectionCount;
	String __fastcall GetHostIpAddress(void);

	void __fastcall RefreshListDisplay();
	void __fastcall FreeConnectedClient(TIdContext *AContext);
	String __fastcall SendClientIp( String iSIP );
	void __fastcall IncConnectionCount();
	void __fastcall DecConnectionCount();
	void __fastcall CloseConnect(TIdContext *AContext);
	String __fastcall HashString(String iStr);
	void __fastcall SendAnswer(TIdIOHandler *AIOHandler, String iQuestion);
	int mix(char m, int s);
	int hash(const char * m);
public:		// User declarations
	void __fastcall AddMemoString ( String iStr );
	__fastcall TfmMyIndy(TComponent* Owner);
};
//---------------------------------------------------------------------------
extern PACKAGE TfmMyIndy *fmMyIndy;
//---------------------------------------------------------------------------
#endif
