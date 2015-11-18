<?m
	int         id;
	const char *name;
?>
<?i
	udata->id = strtol(aw_session_get_param(sess, "id"), NULL, 0);
	udata->name = aw_session_get_param(sess, "name");
?>
<html>
<?= "name:%s id:%d", udata->name, udata->id ?>
</html>
