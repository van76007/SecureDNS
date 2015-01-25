<?php

	include_once 'Settings.php';
	include_once 'autoexec.php';

	class FPReport
	{
	
		var $clientIP = "";
		var $hostname = "";
		
		public function __construct($host, $clientIP)
		{
			$this->clientIP = $clientIP;
			$this->hostname = $host;
		}
		
		
		public function Save()
		{
			$db = new PDO("mysql:host=" . $GLOBALS["dbServer"] . ":3306;dbname=" . $GLOBALS["db"], $GLOBALS["dbUsername"], $GLOBALS["dbPassword"]);
			
			$query = $db->prepare("INSERT INTO fpreports (clientIP, hostname) VALUES (?, ?)");
			$query->execute(array($this->clientIP, $this->hostname));
			
			$db = null;
		}
	
	}
	
?>