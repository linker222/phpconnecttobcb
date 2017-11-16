<?php
	require_once('myfunc.php');
	if (isset($_POST["mode"]))
	{
		$mode = $_POST["mode"];
		if ($mode == "sendquestion")
		{
			//$host    = "10.12.1.44";
			$host    = "10.1.7.102";
			$port    = 8080; 
			$socket = socket_create(AF_INET, SOCK_STREAM, 0) or die("Could not create socket\n");
			/*
			if (!$socket) 
			{
				$err_msg = socket_strerror(socket_last_error($socket));
				socket_close($socket);
				throw new Exception("socket_create() failed:" . $err_msg);
			}
			*/
			$result = socket_connect($socket, $host, $port) or die("Could not connect to server\n");
			/*
			if (!$result) 
			{
				$err_msg = socket_strerror(socket_last_error($socket));
				socket_close($socket);
				throw new Exception("socket_connect() failed:" . $err_msg);
			}
			*/
								
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
			socket_write($socket, $message, strlen($message)) or die("Could not send data to server\n");

			//1 get
			$response = socket_read($socket, 1024) or die("Could not read server response\n");
			/*
			//2 put
			
			$e1 = new Command();
			$e1->mode = $mode;
			$e1->name = $_POST["myname"] . "shing";
			$e1->hobbies = $_POST["myhobbies"];
			$e1->birthdate = $_POST["mybirthdate"];
			$message = json_encode($e1);			
			socket_write($socket, $message, strlen($message)) or die("Could not send data to server\n");
			
			//2 get
			$response = socket_read($socket, 1024) or die("Could not read server response\n");						
			*/
			socket_close($socket);
			
			//$response 為json格式
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