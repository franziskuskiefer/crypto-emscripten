var gcrypt = {};

gcrypt.convert2 = function(func, x, y){
	var numPointerX = Module.allocate(Module.intArrayFromString(x), 'i8', Module.ALLOC_STACK);
	var numPointerY = Module.allocate(Module.intArrayFromString(y), 'i8', Module.ALLOC_STACK);
	return Module.Pointer_stringify(func(numPointerX, numPointerY)); 
}

gcrypt.convert3 = function(func, x, y, z){
	var numPointerX = Module.allocate(Module.intArrayFromString(x), 'i8', Module.ALLOC_STACK);
	var numPointerY = Module.allocate(Module.intArrayFromString(y), 'i8', Module.ALLOC_STACK);
	var numPointerZ = Module.allocate(Module.intArrayFromString(z), 'i8', Module.ALLOC_STACK);
	return Module.Pointer_stringify(func(numPointerX, numPointerY, numPointerZ)); 
}

gcrypt.mul = function(x, y) {
	return gcrypt.convert2(Module._mul, x, y);
}

gcrypt.add = function(x, y) {
	return gcrypt.convert2(Module._add, x, y);
}

gcrypt.sub = function(x, y) {
	return gcrypt.convert2(Module._sub, x, y);
}

gcrypt.modPow = function(x, y, z) {
	return gcrypt.convert3(Module._modPow, x, y, z);
}

gcrypt.inv = function(x, y) {
	return gcrypt.convert2(Module._inv, x, y);
}

gcrypt.rand = function(x) {
	var numPointerX = Module.allocate(Module.intArrayFromString(x), 'i8', Module.ALLOC_STACK);
	return Module.Pointer_stringify(Module._genGcryptRand(numPointerX)); 
}

/*
 * Native SPAKE interface -> spake.next
 */
gcrypt.spakeNext = function(mIn, passHash){
	var mInPointer = Module.allocate(Module.intArrayFromString(mIn), 'i8', Module.ALLOC_STACK);
	var pwdPointer = Module.allocate(Module.intArrayFromString(passHash), 'i8', Module.ALLOC_STACK);
	var start = Date.now();
	var m = Module.Pointer_stringify(Module._spakeNext(mInPointer, pwdPointer));
	var end = Date.now(); 
	console.log("actual time: "+(start-end));
	
	return m.split(",");
}

