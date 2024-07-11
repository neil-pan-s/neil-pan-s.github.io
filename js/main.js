/**
 * Sets up Justified Gallery.
 */
if (!!$.prototype.justifiedGallery) {
  var options = {
    rowHeight: 140,
    margins: 4,
    lastRow: 'justify'
  };
  $('.article-gallery').justifiedGallery(options);
}


$(document).ready(function() {

  /**
   * Shows the responsive navigation menu on mobile.
   */
  $("#header > #nav > ul > .icon").click(function() {
    $("#header > #nav > ul").toggleClass("responsive");
  });


  /**
   * Controls the different versions of  the menu in blog post articles 
   * for Desktop, tablet and mobile.
   */
  if ($(".post").length) {
    /**
     * Display the menu if the menu icon is clicked.
     */
    var menu = $("#menu");
    var menu_icon = $("#menu-icon, #menu-icon-tablet");
    menu_icon.click(function() {
      if (menu.css('visibility') === 'hidden') {
        menu.css("visibility", "visible");
        menu_icon.addClass('active');
      } else {
        menu.css("visibility", "hidden");
        menu_icon.removeClass('active');
      }
      return false;
    });

    /**
     * Add a scroll listener to the menu to hide/show the navigation links.
     */
    if (menu.length) {
      $(window).on('scroll', function() {
        var topDistance = $("#menu > #nav").offset().top;

        // hide only the navigation links on desktop
        if (menu.css('visibility') !== 'hidden' && topDistance < 50) {
          $("#menu > #nav").show();
        } else if (menu.css('visibility') !== 'hidden' && topDistance > 100) {
          $("#menu > #nav").hide();
        }

        // on tablet, hide the navigation icon as well and show a "scroll to top
        // icon" instead
        if ( ! $( "#menu-icon" ).is(":visible") && topDistance < 50 ) {
          $("#menu-icon-tablet").show();
          $("#top-icon-tablet").hide();
        } else if (! $( "#menu-icon" ).is(":visible") && topDistance > 100) {
          $("#menu-icon-tablet").hide();
          $("#top-icon-tablet").show();
        }
      });
    }

    /**
     * Show mobile navigation menu after scrolling upwards,
     * hide it again after scrolling downwards.
     */
    if ($( "#footer-post").length) {
      var lastScrollTop = 0;
      $(window).on('scroll', function() {
        var topDistance = $(window).scrollTop();

        if (topDistance > lastScrollTop){
          // downscroll -> show menu
          $("#footer-post").hide();
        } else {
          // upscroll -> hide menu
          $("#footer-post").show();
        }
        lastScrollTop = topDistance;

        // close all submenu's on scroll
        $("#nav-footer").hide();
        $("#toc-footer").hide();
        $("#share-footer").hide();

        // show a "navigation" icon when close to the top of the page, 
        // otherwise show a "scroll to the top" icon
        if (topDistance < 50) {
          $("#actions-footer > ul > #top").hide();
          $("#actions-footer > ul > #menu").show();
        } else if (topDistance > 100) {
          $("#actions-footer > ul > #menu").hide();
          $("#actions-footer > ul > #top").show();
        }        
      });
    }
  }


  /**
   * Gallery Support
   * 
   */
	(function()
	{	 
    var img_height=470;            //设置图片显示高度
    var img_width=100;			       //设置图片显示宽度

    $(".gallery_img").css("height",img_height+'px');
    $(".gallery_img").load(function(){
        $(this).css("left",function(index,value){
          //console.log($(this).width());
          var hidden_width=-($(this).width()*img_height/$(this).height()-img_width)/2;  //获取图片两边隐藏部分的宽度	
          //console.log(hidden_width);
          return hidden_width+'px';
        });
    })
    
    $(".gallery_a_left").mouseover(function(e){
        $(this).siblings("img").stop(true);
        $(this).siblings("img").animate({"left": -($(this).siblings("img").width() - img_width) + "px"},"slow");
    });

    $(".gallery_a_right").mouseover(function(e){
        $(this).siblings("img").stop(true);
        $(this).siblings("img").animate({"left":"0px"},"slow");
    });

    $(".gallery_a_right").mouseout(function(e){
        $(this).siblings("img").stop(true);
    });

    $(".gallery_a_left").mouseout(function(e){
        $(this).siblings("img").stop(true);
    });

	})();
});
