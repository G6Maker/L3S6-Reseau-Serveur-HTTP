<?php
	// On démarre la session AVANT d'écrire du code HTML
	session_start();
	
	if (isset($_GET["fname"])){
		// On crée la variables du mot de passe dans $_SESSION
		$_SESSION['name']  = $_GET["fname"];
	}

?>

<!DOCTYPE html>
<html lang="fr">
<head>
<meta charset="utf-8" />
<title>Bluepeace</title>
</head>

<body>

	<?php
		$orga = $_GET["orga"];
		$date = $_GET["date"];
		$time = $_GET["time"];
		$mail = $_GET["mail"];
		
		$servername = "inf-mysql.univ-rouen.fr"; /* Info MySQL a modifier pour le lieux d'execution */
		$username = "dourlma2";
		$password = "04012000";
		$dbname = "dourlma22";
	
		$conn = new mysqli($servername, $username, $password, $dbname);
		if (isset($_GET["fname"]) AND isset($_GET["name"])){
			
			$fnom = $_GET["fname"];
			$nom = $_GET["name"];
			
			if ($conn->connect_error) {
				die("Connection failed: " . $conn->connect_error);
			}
	// check if plein, ajouter un EMAIL

			$sql = "INSERT INTO inscription (prenom, nom, orga, mail, date_, time_)
			VALUES ('$fnom', '$nom', '$orga', '$mail', '$date', '$time')";

			if ($conn->query($sql) === TRUE) {
				echo "Votre participation à bien été pris en compte"."<br>";
			} else {
					echo "Error: " . $sql . "<br>" . $conn->error;
			}
			$conn->close();
		} else { 
			echo "Veuillez remplir les champs obligatoires";
		}
	?>
		<form method="post" action="event.php">
    <input type="submit" value="Retour au formulaire">
    </form>
   
</body>
</html>
