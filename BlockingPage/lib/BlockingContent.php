<?php

	include_once 'settings.php';
	include_once 'autoexec.php';

	class BlockedContent
	{
	
		var $ALLOWCONTINUE = false;
		var $hostname = "";
		var $count = 1;
		
		public function __construct($host)
		{
			$this->hostname = $host;
		}
	
		public function CheckRBLS()
		{
			$db = new PDO("mysql:host=" . $GLOBALS["dbServer"] . ":3306;dbname=" . $GLOBALS["db"], $GLOBALS["dbUsername"], $GLOBALS["dbPassword"]);
			
			$html = "";
			
			$query = $db->prepare("SELECT e.reason_name, e.reason_description 
									FROM blockeddomainstats b
									INNER JOIN responses r 
									ON r.response = b.response 
									AND r.zone_name = b.zone_name
									INNER JOIN reasons e 
									ON e.reason_id = r.reason_id
									WHERE b.blockeddomain = ? 
									GROUP BY e.reason_name");
						
			$query->execute(array($this->hostname));
			
			$startCount = $this->count;
									
			foreach($query->fetchAll() as $row)
			{
				if(!empty($row["reason_name"]))
				{	
					if($html == "")
					{
						$html .= "<span class='cleaner'><strong>#" . $this->count . " Reputational</strong><br />";
						$this->count++;
					}
					
					if($startCount >= $this->count)
					{
						$html .= ", ";
					}
					
					$html .= $row["reason_name"] . ": " . str_replace("\r\n", "<br /><br />", $row["reason_description"]);
					$startCount++;
					
					$this->ALLOWCONTINUE = true;
				}
			}
			
			if($html != "")
			{
				$html .= "</span>";
			}
			
			$db = null;
			
			return $html;
		}
		
		public function CheckMalware()
		{
			$db = new PDO("mysql:host=" . $GLOBALS["dbServer"] . ":3306;dbname=" . $GLOBALS["db"], $GLOBALS["dbUsername"], $GLOBALS["dbPassword"]);

			$html = "";
										
			$query = $db->prepare("SELECT r.reason_name, r.reason_description AS rDesc, m.description AS mDesc, f.flavor_name 
									FROM malware m 
									INNER JOIN blockeddomainstats b 
									ON b.domain = m.hostname 
									INNER JOIN unified_responses u
									ON u.unified_response = b.response
									INNER JOIN reasons r
									ON r.reason_id = u.reason_id
									INNER JOIN flavors f
									ON f.flavor_id = u.flavor_id
									WHERE b.blockeddomain = ? AND b.zone_name = '' 
									GROUP BY r.reason_name");
							
			$query->execute(array($this->hostname));
			
			$startCount = $this->count;
									
			foreach($query->fetchAll() as $row)
			{
				if(!empty($row["reason_name"]))
				{	
					if($html == "")
					{
						$html .= "<span class='cleaner'><strong>#" . $this->count . " " . $row["flavor_name"] . "</strong><br />";
						$this->count++;
					}
					
					if($startCount >= $this->count)
					{
						$html .= "<br /><br />";
					}
					
					$html .= $row["reason_name"] . ": " . $row["mDesc"];
					$html .= "<br />" . str_replace("\r\n", "<br /><br />", $row["rDesc"]);
					
					$startCount++;
				}
			}
			
			if($html != "")
			{
				$html .= "</span>";
			}
			
			$db = null;
			
			return $html;
		}
		
		public function CheckCAI()
		{
			$db = new PDO("mysql:host=" . $GLOBALS["dbServer"] . ":3306;dbname=" . $GLOBALS["db"], $GLOBALS["dbUsername"], $GLOBALS["dbPassword"]);

			$html = "";
			
			$query = $db->prepare("SELECT * FROM cai WHERE hostname = ? LIMIT 0,1");
			$query->execute(array($this->hostname));
			
			foreach($query->fetchAll() as $row)
			{
				$html .= "<span class='cleaner'><strong>#" . $this->count . " Child Abuse Imagery</strong></span>";
				$this->count++;
			}
			
			$db = null;
			
			return $html;
		}
		

		public function CheckBlockedIP($ip)
		{
			$db = new PDO("mysql:host=" . $GLOBALS["dbServer"] . ":3306;dbname=" . $GLOBALS["db"], $GLOBALS["dbUsername"], $GLOBALS["dbPassword"]);

			$html = "";
			
			$query = $db->prepare("SELECT * FROM blocked_ips WHERE ip = ?");
			$query->execute(array($ip));
			
			foreach($query->fetchAll() as $row)
			{
				$html .= "<span class='cleaner'><strong>#" . $this->count . " Blocked</strong><br />'" . $ip . "' is blocked<br />" . $row["Reason"] . "</span>";
				$this->count++;
			}

			$db = null;
			
			return $html;

		}
	}

?>