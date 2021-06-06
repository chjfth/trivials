<?php
	$input = $_GET["param"];
	echo "<div>".$input."</div>";

	// [2021-05-26]Chj note: Injecting ?param=GoGogo<script>alert("Haha");<script>
	// will take effect on PHP 5.3, but not on PHP 7.4 .
	error_log("Client-side input is: " . $input);
?>
