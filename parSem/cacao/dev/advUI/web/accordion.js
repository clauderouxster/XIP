/* 
 * Xerox Research Center Europe
 * Copyright (C) 2009 XRCE, All Rights Reserved
 */




            $(document).ready(function(){

                //hide message_body after the first one
                $(".message_list .message_body:gt(0)").hide();

            	//hide message li after the 5th
                $(".message_list li:gt(4)").hide();


                //toggle message_body
                $(".message_head").click(function(){
                    $(this).next(".message_body").slideToggle(500);
                    $(this).toggleClass("active");
                    $(this).siblings("message_head").removeClass("active");
                return false;
            });

            //collapse all messages
            $(".collpase_all_message").click(function(){
               $(".message_body").slideUp(500)
                return false;
            });

            //show all messages
            $(".show_all_message").click(function(){
                $(this).hide()
                $(".show_recent_only").show()
                $(".message_list li:gt(4)").slideDown()
                return false;
            });

        	//show recent messages only
            $(".show_recent_only").click(function(){
                $(this).hide()
                $(".show_all_message").show()
                $(".message_list li:gt(4)").slideUp()
                return false;
        });

    });



            $(document).ready(function(){

                //hide message_body after the first one
                $(".document_list .document_body:gt(0)").hide();

            	//hide message li after the 5th
                $(".document_list li:gt(4)").hide();


                //toggle message_body
                $(".document_head").click(function(){
                    $(this).next(".document_body").slideToggle(500);
                    $(this).toggleClass("active");
                    $(this).siblings("document_head").removeClass("active");
                return false;
            });

            //collapse all messages
            $(".collpase_all_message").click(function(){
                $(".document_body").slideUp(500)
                return false;
            });

            //show all messages
            $(".show_all_message").click(function(){
                $(this).hide()
                $(".show_recent_only").show()
                $(".document_list li:gt(4)").slideDown()
                return false;
            });

        	//show recent messages only
            $(".show_recent_only").click(function(){
                $(this).hide()
                $(".show_all_message").show()
                $(".document_list li:gt(4)").slideUp()
                return false;
        });

        });
