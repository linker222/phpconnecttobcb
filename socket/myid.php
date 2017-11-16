<?php
	function gen_uuid($len=8) 
	{
		$hex = md5("yourSaltHere" . uniqid("", true));
		$pack = pack('H*', $hex);
		$tmp =  base64_encode($pack);
		$uid = preg_replace("#(*UTF8)[^A-Za-z0-9]#", "", $tmp);
		$len = max(4, min(128, $len));
		while (strlen($uid) < $len)
			$uid .= gen_uuid(22);
		return substr($uid, 0, $len);
	}
	function gen_uid($l=10)
	{
		return substr(str_shuffle("0123456789abcdefghijklmnopqrstuvwxyz"), 0, $l);
	}
	function gen_uuid1($len=8)
	{
		$hex = md5("your_random_salt_here_31415" . uniqid("", true));

		$pack = pack('H*', $hex);

		$uid = base64_encode($pack);        // max 22 chars

		$uid = ereg_replace("[^A-Za-z0-9]", "", $uid);    // mixed case
		//$uid = ereg_replace("[^A-Z0-9]", "", strtoupper($uid));    // uppercase only

		if ($len<4)
			$len=4;
		if ($len>128)
			$len=128;                       // prevent silliness, can remove

		while (strlen($uid)<$len)
			$uid = $uid . gen_uuid1(22);     // append until length achieved
		return substr($uid, 0, $len);
	}
	//$myid = toUId(10000, 11111);
	//$myid1 = fromUId($myid, 11111);
	function toUId($baseId, $multiplier = 1) 
	{
		return base_convert($baseId * $multiplier, 10, 36);
	}
	function fromUId($uid, $multiplier = 1) 
	{
		return (int) base_convert($uid, 36, 10) / $multiplier;
	}
	
	function strip_digits_from_string($string) 
	{
		return preg_replace('/[0-9]/', '', $string);
	}
	function replace_digits_with_letters($string) 
	{
		return strtr($string, '01234567890', 'abcdefghij');
	}
	
?>
