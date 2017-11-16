<?php
class Connector
{
	public static $instance = null;
	public $conn;
	private function __construct($ip, $port)
	{
		set_time_limit(0);
		//$ip = "10.12.1.44";
		//$port = 8080;
		if (($this->conn = socket_create(AF_INET, SOCK_STREAM, SOL_TCP)) < 0)
		{
			//echo "socket_create() 失敗的原因是:" . socket_strerror($this->conn)."\n";
			die("Unable to create socket");
		}
		
		if (socket_set_nonblock($this->conn)) 
		{
			/*
			$result = @socket_connect($this->conn, $ip, $port);
			if ($result || socket_last_error() == EINPROGRESS) 
			{
				$errno = EINPROGRESS;
				return $this->conn;
			}
			*/
			
			$result = @socket_connect($this->conn, $ip, $port);
			if ($result < 0) 
			{
				//echo "socket_connect() failed.\nReason: ($result) " . socket_strerror($result) . "\n";
				die("Unable to connect socket");
			}						
		}
		else
		{
			die("Unable to set nonblocking mode for socket");
		}
		/*
		$errno = socket_last_error($this->conn);
		$errstr = socket_strerror($errno);
		socket_close($this->conn);
		return false;		
		*/
	}
	public static function getInstance($ip, $port)
	{
		if (is_null(self::$instance))
		{
		  self::$instance = new Connector($ip, $port);
		}
		return self::$instance;
	}
	public function sendMsg($msg)
	{
		socket_write($this->conn, $msg);
	}
	public function getMsg()
	{
		$clients = array($this->conn);
		while (true)
		{
			$read = $clients;
			$wrSet = NULL;
			$errSet = NULL;
			if (socket_select($read, $wrSet,$errSet, 3) < 1)
			{
				continue;
			}
			foreach ($read as $read_sock)
			{
				$data = @socket_read($read_sock, 1024, PHP_BINARY_READ);
				socket_close($this->conn);
				return $data;
			}
		}
	}
}
?>
