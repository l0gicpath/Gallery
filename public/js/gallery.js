$(document).ready(function () {
	//$("#thumbnails .thumbnail a").lightBox({fixedNavigation:true});
	$('#accountPicker').change(function () {
		$.post('/account',{eml: $(this).val()}, function (r) {
			if (r.success) window.location.href = '/';
		}, 'json');
	});
});