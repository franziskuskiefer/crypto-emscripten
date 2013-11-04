//  <script src="../node_modules/bigint.js"></script>
var Module = {
	'print': function(text) {
		console.log(text)
	}
	//'noInitialRun' : true
};
Module.arguments = ["--verbose"];

(function(){
        {{{ SCRIPT_CODE }}}
})({"Module":Module,"crypto":crypto,"BigInt":BigInt});

