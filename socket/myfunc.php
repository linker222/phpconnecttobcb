<?php
	function my_ip($dest='64.0.0.0', $port=80)
	{
		$socket = socket_create(AF_INET, SOCK_DGRAM, SOL_UDP);
		socket_connect($socket, $dest, $port);
		socket_getsockname($socket, $addr, $port);
		socket_close($socket);
		return $addr;
	}
	//$pingvalue = ping("10.12.1.44");
	function ping($host) 
	{
		$package = "\x08\x00\x19\x2f\x00\x00\x00\x00\x70\x69\x6e\x67";
		/* create the socket, the last '1' denotes ICMP */    
		$socket = socket_create(AF_INET, SOCK_RAW, 1);    
		/* set socket receive timeout to 1 second */
		socket_set_option($socket, SOL_SOCKET, SO_RCVTIMEO, array("sec" => 1, "usec" => 0));    
		/* connect to socket */
		socket_connect($socket, $host, null);	    
		/* record start time */
		list($start_usec, $start_sec) = explode(" ", microtime());
		$start_time = ((float) $start_usec + (float) $start_sec);    
		socket_send($socket, $package, strlen($package), 0);    
		if(@socket_read($socket, 255)) 
		{
			list($end_usec, $end_sec) = explode(" ", microtime());
			$end_time = ((float) $end_usec + (float) $end_sec);

			$total_time = $end_time - $start_time;

			return $total_time;
		} 
		else 
		{
			return false;
		}
		socket_close($socket);
	}	
	function json_decode_nice($json, $assoc = FALSE)
	{
		$json = str_replace(array("\n","\r"),"", $json);
		$json = preg_replace('/([{,]+)(\s*)([^"]+?)\s*:/','$1"$3":', $json);
		$json = preg_replace('/(,)\s*}$/','}', $json);
		return json_decode($json, $assoc);
	}
?>
