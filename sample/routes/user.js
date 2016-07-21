"use strict";

/*
 * GET users listing.
 */

exports.list = function(req, res){

    res.format({
        text: function(){
            res.send( 'hey');
        },

        html: function(){
            res.send('<p>hey</p>');
        },

        json: function(){
            res.send({ message: 'hey' });
        }
    });


};