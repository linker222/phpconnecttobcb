<html>
    <head>
        <meta charset="utf-8">
        <title>test</title>
    </head>
    <body>

        <form action="" method="post">
        姓名:<input type="text" name="name"><br >
        興趣:<input type="text" name="hobbies"><br >
        生日:<input type="text" name="birthdate"><br >
        <input type="submit" value="提交" name="submit">
        </form>
    </body>
</html>


<?php
function json_decode_nice($json, $assoc = FALSE)
{
	$json = str_replace(array("\n","\r"),"", $json);
	$json = preg_replace('/([{,]+)(\s*)([^"]+?)\s*:/','$1"$3":', $json);
	$json = preg_replace('/(,)\s*}$/','}', $json);
	return json_decode($json, $assoc);
}
	if(isset($_POST['submit']))
	{
		$host    = "10.1.7.102";
		$port    = 8080; 
		// create socket
		$socket = socket_create(AF_INET, SOCK_STREAM, 0) or die("Could not create socket\n");
		// connect to server
		$result = socket_connect($socket, $host, $port) or die("Could not connect to server\n");  

		class Emp
		{
			public $name = "";
			public $hobbies = "";
			public $birthdate = "";
		}
		
		$e = new Emp();
		$e->name = $_POST["name"];
		$e->hobbies = $_POST["hobbies"];
		$e->birthdate = $_POST["birthdate"];

		$message = json_encode($e);
		
		#列印一些資訊
		/*
		echo $message; #json串
		echo '<br >';  
		echo strlen($message);#json長度
		echo '<br >';
		*/
		
		// send string to server
		socket_write($socket, $message, strlen($message)) or die("Could not send data to server\n");
		
		// get server response
		$result = socket_read ($socket, 1024) or die("Could not read server response\n");			
		
		// close socket		
		socket_close($socket);
				
		$obj = json_decode_nice($result);		
		echo "\tReply From Server  : " . $obj->{'name'};
		//if ( ob_get_length() ) ob_clean();
		/*
		header('Expires: Mon, 26 Jul 1997 05:00:00 GMT');
		header('Last-Modified:' . gmdate('D, d M Y H:i:s') . 'GMT');
		header('Cache-Control: no-cache, must-revalidate');
		header('Pragma: no-cache');
		header('Content-Type: application/json');
		echo json_encode($response);		
		*/		
	}
?>