<?php
	
	include_once 'settings.php';
	include_once 'autoexec.php';

	class Endpoint
	{
	
		var $hostname = "";
		var $ip = "";
		var $customerID = "";
		var $MAC = "";
		var $publicHostname = "";
	
		
		public function __construct($hostname, $ip, $customerID, $MAC, $publicHostname)
		{
			$this->hostname = $hostname;
			$this->ip = $ip;
			$this->customerID = $customerID;
			$this->MAC = $MAC;
			$this->publicHostname = $publicHostname;
		}
	
	
		public function Register()
		{
			$db = new PDO("mysql:host=" . $GLOBALS["dbServer"] . ":3306;dbname=" . $GLOBALS["db"], $GLOBALS["dbUsername"], $GLOBALS["dbPassword"]);
			
			$query = $db->prepare("INSERT INTO customer_endpoints (ip, name, customer_id, MAC, public_hostname) VALUES (?, ?, ?, ?, ?) ON DUPLICATE KEY UPDATE modified = CURRENT_TIMESTAMP, occurrences = occurrences+1");
			$query->execute(array($this->ip, $this->hostname, $this->customerID, $this->MAC, $this->publicHostname));
			
			$db = null;
		}
		
	}
	
?>