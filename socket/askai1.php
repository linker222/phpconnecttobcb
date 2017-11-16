<?php
	require_once('mysocket.php');
	require_once('myfunc.php');
	require_once('myid.php');
	if (isset($_POST["mode"]))
	{
		$mode = $_POST["mode"];
		if ($mode == "sendquestion")
		{
			//$ip = my_ip();
			$ip = "10.1.7.102";
			//$pingvalue = ping("10.12.1.44");		
			//$ip = gethostbyname(parse_url("capupload.nace.edu.tw", PHP_URL_HOST));
			$con = Connector::getInstance($ip, 8080);							
			class Command
			{
				public $mode = "";
				public $name = "";
				public $hobbies = "";
				public $birthdate = "";
			}	
			//1 put
			$e = new Command();
			$e->mode = $mode;
			$e->name = $_POST["myname"];
			$e->hobbies = $_POST["myhobbies"];
			$e->birthdate = $_POST["mybirthdate"];
			$message = json_encode($e);
			
			$con->sendMsg($message);
			$response = $con->getMsg();
			$obj = json_decode_nice($response);
			
			$response1 = array();
			$response1["author"] = $obj->{"author"};
			$response1["title"] = $obj->title;
			$response1["subtitle"] = $obj->subtitle;
	
			if ( ob_get_length() ) ob_clean();
			header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');
			header('Last-Modified:' . gmdate('D, d M Y H:i:s') . 'GMT');
			header('Cache-Control: no-cache, must-revalidate');
			header('Pragma: no-cache');
			header('Content-Type: application/json');
			echo json_encode($response1);		
			//echo $response;
		}
	}
?>