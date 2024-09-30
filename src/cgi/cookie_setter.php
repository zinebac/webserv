<?php
// example of setting a cookie in PHP

// Get the current timestamp
$expirationTime = time() + 3600; // Expires in 1 hour

// Generate a random value for the cookie
$cookieValue = uniqid('user_', true);

// Set the cookie in the response
setcookie('my_cookie', $cookieValue, $expirationTime);

// Output the "Set-Cookie" header to the browser
header('Set-Cookie: my_cookie=' . $cookieValue . '; expires=' . gmdate('D, d M Y H:i:s T', $expirationTime) . '; path=/');

// Display a message
echo "Cookie set successfully!";
?>
