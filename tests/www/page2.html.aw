<?h
#include <sys/types.h>
#include <dirent.h>
?>
<html>
	<?c
	DIR *dir = opendir("/");
	struct dirent *ent;

	do {
		ent = readdir(dir);
		if (ent) {
	?>
		<?= ent->d_name ?>
		<?c if (ent->d_type == DT_DIR) ?>
			[DIR]
		<br/>
	<?c
		}
	} while (ent);
	closedir(dir);
	?>
	<button onclick="window.location.href='sub/'">next</button>
</html>
