<?php

	include_once 'lib/DynDNS.lib.php';
	include_once 'lib/Settings.php';
	
	if(isset($_GET['hostname']) && isset($_GET['id']) && isset($_GET['mac']))
	{
		$hostname = $_GET['hostname'];
		$customerID = $_GET['id'];
		$ip = $_SERVER["REMOTE_ADDR"];
		$macAdress = $_GET['mac'];
		$publicHostname = "";
		
		if(isset($_GET['ph']))
		{
			$publicHostname = $_GET['ph'];
		}
		
		$endpoint = new Endpoint($hostname, $ip, $customerID, $macAdress, $publicHostname);
		$endpoint->Register();
	}
	
?>