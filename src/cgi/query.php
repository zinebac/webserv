<?php
  // Get the values of the query string parameters
  $name = isset($_GET['name']) ? $_GET['name'] : '';
  $age = isset($_GET['age']) ? $_GET['age'] : '';
  $occupation = isset($_GET['occupation']) ? $_GET['occupation'] : '';

  // Sanitize the retrieved values
  $name = htmlentities($name);
  $age = htmlentities($age);
  $occupation = htmlentities($occupation);

  // Display the sanitized values
  echo "Name: $name<br>";
  echo "Age: $age<br>";
  echo "Occupation: $occupation<br>";
?>

