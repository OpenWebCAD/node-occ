var utils = utils || {};
utils.Spinner = function() {

    var stackSize = 0;

    this.show = function() {
        if (stackSize === 0) {
            if ($('#progress-container').children().size() == 0) {
                $('#progress-container').append(
                    '<div id="progress"><img src="/images/progress-spinner.gif" alt="in progress"/></div>');
            }
            $('#progress').show();
        }
        ++stackSize;
    }

    this.hide = function() {
        --stackSize;
        if (stackSize == 0) {
            $('#progress').hide();
        }
    }

}
utils.spinner = new SS.Spinner();
utils.withSpinner = function(closure) {
    utils.spinner.show();
    try {
        closure();
    } catch (e) {
        console.error(e);
    }
    utils.spinner.hide();
}