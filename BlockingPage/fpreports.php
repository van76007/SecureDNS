<?php

	include_once 'lib/FPReports.php';

	session_start(); 
	
	if(isset($_SESSION['fp']) && isset($_SESSION['returnUrl']))
	{
		$fpReport = new FPReport($_SESSION['fp'], $_SERVER["REMOTE_ADDR"]);
		$fpReport->Save();
		
		$url = urldecode($_SESSION['returnUrl']);
		
		$bits = parse_url($url);
		
		$returnUrl = "";
		
		if(isset($bits["query"]))
		{
			$returnUrl = $url . "&subfp=1";
		}
		else
		{
			$returnUrl = $url . "?subfp=1";
		}

		$html = "Your report has been submitted successfully. Thank you!";
		
		if(isset($_SESSION['rbl']))
		{
			$html .= "<br /><br /><a href='$returnUrl'>Return to the blocking page</a> and click on continue if you feel that you should not have been blocked.";
		}
		
		include 'inc/header.fp.inc';
		echo $html;
		include 'inc/footer.inc';
		
		//header("Location: " . $returnUrl . "&subfp=1");
		//exit();
	}
	
?>