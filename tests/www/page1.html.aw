<?h
#include <time.h>
?>
<?m
	int age;
	int male;
	const char *name;
?>
<?i
	const char *str;

	str = aw_session_get_param(sess, "age");
	udata->age = str ? strtol(str, NULL, 0) : 0;
	str = aw_session_get_param(sess, "male");
	udata->male = str ? 1 : 0;
	str = aw_session_get_param(sess, "name");
	udata->name = str ? str : "unknown";
?>
<html>
	<?c
		time_t now;
		struct tm *date;
		int year;
	?>
	Hello, <?= "%s",udata->name ?>
	<br/>
	Hi,
	<?c if (udata->male) { ?>
		Boy
	<?c } else { ?>
		Girl
	<?c } ?>
	<br/>

	<?c
		time(&now);
		date = localtime(&now);
		year = date->tm_year - udata->age;
	?>

	You were born in <?= "%d",year+1900 ?>
	<br/>
	<button onclick="window.location.href='page2.html'">next</button>
</html>
